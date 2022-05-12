#include "BatteryCommands.h"
#include <string.h> //Без этого не работает memcpy
#include "QDebug"


UINT16 ReadWord(HID_SMBUS_DEVICE device, INT *data, INT battery_register)
{
    BOOL                opened=1;               //Открылось ли устройство
    HID_SMBUS_S0        status0;                //Расширенный статус
    HID_SMBUS_S1        status1;                //Расширенный статус
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт
    BYTE                numBytesToRead = 2;     //Количество байт которые нужно прочитать
    BYTE                numBytesRead = 0;       //Количество прочитанных байт
    WORD                targetAddressSize = 1;  //Количество байт в адресе
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                targetAddress[16]={0};  //Адрес для чтения

    targetAddress[0] = battery_register;

    HidSmbus_IsOpened(device, &opened);
    if(opened)
    {
        if (HidSmbus_AddressReadRequest(device, BATTERY_ADDRESS << 1, numBytesToRead, targetAddressSize, targetAddress)!=HID_SMBUS_SUCCESS)return FALSE;
        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;
        if (HidSmbus_GetTransferStatusResponse(device, &status0, &status1, &numRetries, &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;
        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
        if (HidSmbus_ForceReadResponse(device, numBytesToRead)!=HID_SMBUS_SUCCESS)return FALSE;
        if (HidSmbus_GetReadResponse(device, &status0, buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE, &numBytesRead)!=HID_SMBUS_SUCCESS)return FALSE;
        *data = (buffer[1] << 8) | buffer[0];
        return TRUE;
    }
    return FALSE;
}

UINT16 WriteWord(HID_SMBUS_DEVICE device, UINT16 send_data, BYTE battery_register)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    const BYTE          dataLen = 3;
    BYTE                buffer[dataLen]={0};

    buffer[0] = battery_register;
    buffer[1] = (BYTE)send_data & 0xff;
    buffer[2] = send_data >> 8;

    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        status = HidSmbus_WriteRequest(device, BATTERY_ADDRESS << 1, buffer, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
        do
        {
        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;
        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
        }while (status0 !=HID_SMBUS_S0_COMPLETE);

        return TRUE;
    }
    return FALSE;
}

UINT16 ReadTextBlock(HID_SMBUS_DEVICE device, char *block, INT *block_length, INT battery_register)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                numBytesRead;           //Количество прочитанных байт HidSmbus_GetReadResponse
    BYTE                numBytesToRead=MAX_SMBUS_BLOCK_SIZE;      //Сколько байт нужно прочитать
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    WORD                targetAddressSize = 1;  //Количество байт в адресе
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                temp [MAX_SMBUS_BLOCK_SIZE]={0};          //Временный массив для сдвига
    BYTE                targetAddress[16]={0};  //Адрес откуда читать

    targetAddress[0] = battery_register;        //Передаем адрес в массив

    HidSmbus_IsOpened(device, &opened);         //Открываем устройство
    if(opened)                                  //Если открыто
    {
        if (HidSmbus_AddressReadRequest(device, BATTERY_ADDRESS << 1,
                                        numBytesToRead, targetAddressSize,
                                        targetAddress)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;

        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }

        if (HidSmbus_ForceReadResponse(device, numBytesToRead)!=HID_SMBUS_SUCCESS)return FALSE;

            do
            {
                if (HidSmbus_GetReadResponse(device, &status0,
                                             buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE,
                                             &numBytesRead)!= HID_SMBUS_SUCCESS) return FALSE;

                memcpy(&temp[totalNumBytesRead], buffer, numBytesRead); //Копируем часть прочитанных данных во временный
                                                                        //буфер, начиная с адреса totalNumBytesRead
                totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
            } while (totalNumBytesRead < numBytesToRead);               //Все ли мы прочитали

        INT length = temp[0];           //Нулевая ячейка временного буффера содержит длину строки
        if (length > MAX_SMBUS_BLOCK_SIZE) length = MAX_SMBUS_BLOCK_SIZE;   //Если поймали глюк и длина слишком большая, ограничим
        memcpy(&block[0], &temp[1], length); //Сдвинем весь массив на одну ячейку влево (уберем нулевую ячейку) 1й способ
//        for(BYTE i=0; i<length; i++)    //Сдвинем весь массив на одну ячейку влево (уберем нулевую ячейку) 2й способ
//        {
//            block[i] = temp[i+1];
//        }

        block [length]='\0';            //Устанавливаем признак конца строки
        *block_length = length;         //Вернем длину строки

        return TRUE;
    }

    return FALSE;
}

UINT16 ReadDataBlock(HID_SMBUS_DEVICE device, BYTE *block, INT *block_length, INT battery_register)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                numBytesRead;           //Количество прочитанных байт HidSmbus_GetReadResponse
    BYTE                numBytesToRead=MAX_SMBUS_BLOCK_SIZE;      //Сколько байт нужно прочитать
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    WORD                targetAddressSize = 1;  //Количество байт в адресе
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                temp [MAX_SMBUS_BLOCK_SIZE]={0};      //Временный массив для сдвига
    BYTE                targetAddress[16]={0};  //Адрес откуда читать

    targetAddress[0] = battery_register;        //Передаем адрес в массив

    HidSmbus_IsOpened(device, &opened);         //Открываем устройство
    if(opened)                                  //Если открыто
    {
        if (HidSmbus_AddressReadRequest(device, BATTERY_ADDRESS << 1,
                                        numBytesToRead, targetAddressSize,
                                        targetAddress)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;

        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }

        if (HidSmbus_ForceReadResponse(device, numBytesToRead)!=HID_SMBUS_SUCCESS)return FALSE;
            do
            {
                if (HidSmbus_GetReadResponse(device, &status0,
                                             buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE,
                                             &numBytesRead)!= HID_SMBUS_SUCCESS) return FALSE;

                memcpy(&temp[totalNumBytesRead], buffer, numBytesRead); //Копируем часть прочитанных данных во временный
                                                                        //буфер, начиная с адреса totalNumBytesRead
                totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
            } while (totalNumBytesRead < numBytesToRead);               //Все ли мы прочитали

        INT length = temp[0];           //Нулевая ячейка временного буффера содержит длину строки
        if (length > MAX_SMBUS_BLOCK_SIZE) length = MAX_SMBUS_BLOCK_SIZE;   //Если поймали глюк и длина слишком большая, ограничим
        memcpy(&block[0], &temp[1], length); //Сдвинем весь массив на одну ячейку влево (уберем нулевую ячейку) 1й способ
//        for(BYTE i=0; i<length; i++)    //Сдвинем весь массив на одну ячейку влево (уберем нулевую ячейку) 2й способ
//        {
//            block[i] = temp[i+1];
//        }
//      Тут это не понадобится, так как на выходе будет не String, а просто массив
//        block [length]='\0';            //Устанавливаем признак конца строки
        *block_length = length;         //Вернем длину массива

        return TRUE;
    }

    return FALSE;
}

UINT16 ReadFlash(HID_SMBUS_DEVICE device, UINT16 regAddr, BYTE dataLen, BYTE *data)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                numBytesRead;           //Количество прочитанных байт HidSmbus_GetReadResponse
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                address[FLASH_ADDRESS_SIZE]={0};
    BYTE                magic_word [2] = {0xff, 0x03}; // 0xff - наверное команда чтения флеш, 0x03 - количество отправляемых байт

    address[0] = magic_word[0];
    address[1] = magic_word[1];
    address[2] = (BYTE)regAddr&0xff;
    address[3] = regAddr>>8;
    address[4] = dataLen;

//    writeRequest(device, address, sizeof(address));
//    readRequest(device, data, dataLen);

    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write transfer request
        status = HidSmbus_WriteRequest(device, BATTERY_ADDRESS << 1, address, FLASH_ADDRESS_SIZE);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;

        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }

        status = HidSmbus_ReadRequest(device, BATTERY_ADDRESS << 1, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;

        status = HidSmbus_ForceReadResponse(device, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;

        do
        {
            status = HidSmbus_GetReadResponse(device, &status0,
                                              buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE,
                                              &numBytesRead);

            if (status!=HID_SMBUS_SUCCESS)return FALSE;
            memcpy(&data[totalNumBytesRead], buffer, numBytesRead); //Копируем часть прочитанных данных во временный
                                                                    //буфер, начиная с адреса totalNumBytesRead
            totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
        } while (totalNumBytesRead < dataLen);                      //Все ли мы прочитали

        return TRUE;
    }
    return FALSE;
}

UINT16 WriteFlash(HID_SMBUS_DEVICE device, UINT16 regAddr, BYTE dataLen, BYTE *data)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения HidSmbus_GetTransferStatusResponse
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер
    BYTE                magic_word [2] = {0xff, 0x01}; // 0xff - наверное команда доступа во флеш, 0x01 - команда записи
    if(regAddr==0x012A) magic_word [1] = 0x00;//если адрес последний, то вместо 0x01ff отправляем 0x00ff.
    //Без этого данные не сохраняются во флеш

    buffer[0] = magic_word[0]; //Команда доступа во флеш
    buffer[1] = magic_word[1]; //Команда записи
    buffer[2] = (BYTE)regAddr&0xff; //Адрес регистра для записи
    buffer[3] = regAddr>>8; //Адрес регистра для записи
    buffer[4] = dataLen; //Длина записываемых данных
    memcpy(&buffer[5], data, dataLen);

//    writeRequest(device, buffer, 5+dataLen);


    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write transfer request
        status = HidSmbus_WriteRequest(device, BATTERY_ADDRESS << 1, buffer, 5+dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;
        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;

        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

UINT16 writeRequest(HID_SMBUS_DEVICE device, BYTE *send_data, BYTE dataLen)
{
    BOOL                opened;                 //Открылось ли устройство
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    HID_SMBUS_S1        status1;                //Расширенный статус 1
    WORD                numRetries;             //Количество повторов чтения
    WORD                bytesRead;              //Количество прочитанных байт HidSmbus_GetTransferStatusResponse

    if (HidSmbus_IsOpened(device, &opened) == HID_SMBUS_SUCCESS && opened)
    {
        // Issue write transfer request
        status = HidSmbus_WriteRequest(device, BATTERY_ADDRESS << 1, send_data, dataLen);
        if (status!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_TransferStatusRequest(device)!=HID_SMBUS_SUCCESS)return FALSE;

        if (HidSmbus_GetTransferStatusResponse(device, &status0,
                                               &status1, &numRetries,
                                               &bytesRead)!=HID_SMBUS_SUCCESS)return FALSE;

        if (status1 == HID_SMBUS_S1_BUSY_ADDRESS_NACKED)
        {
            HidSmbus_Close(device);
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

UINT16 readRequest(HID_SMBUS_DEVICE device, BYTE *return_data, BYTE dataLen)
{
    HID_SMBUS_STATUS    status;
    HID_SMBUS_S0        status0;                //Расширенный статус 0
    BYTE                numBytesRead;           //Количество прочитанных байт HidSmbus_GetReadResponse
    BYTE                totalNumBytesRead = 0;  //Сколько всего байт прочитано
    BYTE                buffer[HID_SMBUS_MAX_READ_RESPONSE_SIZE]={0}; //Приемный буфер

    status = HidSmbus_ReadRequest(device, BATTERY_ADDRESS << 1, dataLen);
    if (status!=HID_SMBUS_SUCCESS)return FALSE;

    status = HidSmbus_ForceReadResponse(device, dataLen);
    if (status!=HID_SMBUS_SUCCESS)return FALSE;

    do
    {
        status = HidSmbus_GetReadResponse(device, &status0,
                                          buffer, HID_SMBUS_MAX_READ_RESPONSE_SIZE,
                                          &numBytesRead);

        if (status!=HID_SMBUS_SUCCESS)return FALSE;
        memcpy(&return_data[totalNumBytesRead], buffer, numBytesRead); //Копируем часть прочитанных данных во временный
                                                                //буфер, начиная с адреса totalNumBytesRead
        totalNumBytesRead += numBytesRead;                      //Увеличиваем адрес на величину прочитанных байт
    } while (totalNumBytesRead < dataLen);

    return TRUE;
}
