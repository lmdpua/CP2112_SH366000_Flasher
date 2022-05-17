#include "cp2112.h"
#include "cp2112_config.h"
#include <QFile>
//#include <QDir>



//#include "config.h"

//bool defaultSettings = true;

config cp2112config[]=
{
    {BITRATE_HZ,            0},
    {ACK_ADDRESS,           0},
    {AUTO_RESPOND,          0},
    {WRITE_TIMEOUT_MS,      0},
    {READ_TIMEOUT_MS,       0},
    {TRANSFER_RETRIES,      0},
    {SCL_LOW_TIMEOUT,       0},
    {RESPONSE_TIMEOUT_MS,   0},
    {RX_TX_LEDS_ON,         0},
};

quint8 numcp2112config = sizeof (cp2112config)/sizeof (cp2112config[0]);

STATUS CP2112_Init(HID_SMBUS_DEVICE *device, QString cond[])
{
//    readINI();
    HID_SMBUS_STATUS    status;
    DWORD               numDevices  = -1;   //Количество найденных устройств
    INT                 devNumber   = 0;    //Пока нет реализации выбора из нескольких устройств,
                                            //считаем, что устройство одно, под номером 0
    BOOL                opened;             //Открылось ли устройство
    QString             str;
    CP2112_Clear_Condition(cond); //Очистим все состояния

    status = HidSmbus_GetNumDevices(&numDevices, VID, PID);
    if (status !=HID_SMBUS_SUCCESS)
    {
        cond[CONNECTED]=CP2112_Error_Handler(status);
        return STATUS_FAIL;
    }

    if (numDevices==0)
    {
        cond[CONNECTED]="CP2112 not connected";
        return STATUS_FAIL;
    }

    status = HidSmbus_Open(device, devNumber, VID, PID);
    if (status != HID_SMBUS_SUCCESS)
    {
        cond[CONNECTED]=CP2112_Error_Handler(status);
        return STATUS_FAIL;
    }

    status = HidSmbus_IsOpened(*device, &opened);
    if (status != HID_SMBUS_SUCCESS || !opened)
    {
        cond[CONNECTED]=CP2112_Error_Handler(status);
        return STATUS_FAIL;
    }


        //Передаем user конфиг на устройство
        status =  HidSmbus_SetSmbusConfig(*device,
                                          cp2112config[Bitrate].userData.toUInt(),
                                          cp2112config[ACK_Address].userData.toUInt(),
                                          cp2112config[Auto_Respond].userData.toUInt(),
                                          cp2112config[Write_Timeout].userData.toUInt(),
                                          cp2112config[Read_Timeout].userData.toUInt(),
                                          cp2112config[SCL_Low_Timeout].userData.toUInt(),
                                          cp2112config[Transfer_Retries].userData.toUInt()
                                          );

        if (status != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status =  HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }

        //Теперь читаем конфиг с устройства, чтобы убедится, что он записался
        DWORD bitRate;              //Для этого нужно объявить кучу переменных
        BYTE address;
        BOOL autoReadRespond;
        WORD writeTimeout;
        WORD readTimeout;
        BOOL sclLowTimeout;
        WORD transferRetries;
        //И вот только теперь читаем конфиг
        status =  HidSmbus_GetSmbusConfig(*device,
                                          &bitRate,
                                          &address,
                                          &autoReadRespond,
                                          &writeTimeout,
                                          &readTimeout,
                                          &sclLowTimeout,
                                          &transferRetries);
        //Если конфиг передался с ошибкой


        //Передаем таймаут конфиг на устройство
        status = HidSmbus_SetTimeouts(*device, cp2112config[Response_Timeout].defaultData.toUInt());
        if (status != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }

        DWORD response_timeout_ms;

        status = HidSmbus_GetTimeouts(*device, &response_timeout_ms);
        if (status              != HID_SMBUS_SUCCESS    ||
                response_timeout_ms != RESPONSE_TIMEOUT_MS  )
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }


        //Подготовим конфигурацию GPIO
        BYTE    direction   = 0x00; //is a bitmask that specifies each GPIO pin’s direction.
        BYTE    mode        = 0x00; //is a bitmask that specifies each GPIO pin’s mode.
        BYTE    function    = 0x00; //is a bitmask that specifies the special behavior of GPIO.0, GPIO.1, and GPIO.7
        BYTE    m_clkDiv    = 0x00; //is the clock output divider value used for GPIO.7 when configured in clock output mode.

        //Если направление GPIO - выход, то включаем бит по маске
        if (GPIO_0)     direction |= HID_SMBUS_MASK_GPIO_0;
        if (GPIO_1)     direction |= HID_SMBUS_MASK_GPIO_1;
        if (GPIO_2)     direction |= HID_SMBUS_MASK_GPIO_2;
        if (GPIO_3)     direction |= HID_SMBUS_MASK_GPIO_3;
        if (GPIO_4)     direction |= HID_SMBUS_MASK_GPIO_4;
        if (GPIO_5)     direction |= HID_SMBUS_MASK_GPIO_5;
        if (GPIO_6)     direction |= HID_SMBUS_MASK_GPIO_6;
        if (GPIO_7)     direction |= HID_SMBUS_MASK_GPIO_7;

        //Если режим GPIO - push-pull, то включаем бит по маске
        if (GPIO_0_MODE)	mode |= HID_SMBUS_MASK_GPIO_0;
        if (GPIO_1_MODE)	mode |= HID_SMBUS_MASK_GPIO_1;
        if (GPIO_2_MODE)	mode |= HID_SMBUS_MASK_GPIO_2;
        if (GPIO_3_MODE)	mode |= HID_SMBUS_MASK_GPIO_3;
        if (GPIO_4_MODE)	mode |= HID_SMBUS_MASK_GPIO_4;
        if (GPIO_5_MODE)	mode |= HID_SMBUS_MASK_GPIO_5;
        if (GPIO_6_MODE)	mode |= HID_SMBUS_MASK_GPIO_6;
        if (GPIO_7_MODE)	mode |= HID_SMBUS_MASK_GPIO_7;

        //Если используется специальный режим GPIO, то включаем бит по маске
        //В расчет берутся только GPIO_0, GPIO_1, GPIO_7.
        //Для остальных GPIO специальный режим отсутствует
        if(cp2112config[RX_TX_Leds_On].userData==true)
        {
            function |= HID_SMBUS_MASK_FUNCTION_GPIO_0_TXT;
            function |= HID_SMBUS_MASK_FUNCTION_GPIO_1_RXT;
        }
//        if (TX)	function |= HID_SMBUS_MASK_FUNCTION_GPIO_0_TXT;
//        if (RX)	function |= HID_SMBUS_MASK_FUNCTION_GPIO_1_RXT;

        status = HidSmbus_SetGpioConfig(*device, direction, mode, function, m_clkDiv);
        if (status != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }

        //Проверим как записался конфиг GPIO

        BYTE    _direction   = 0x00; //is a bitmask that specifies each GPIO pin’s direction.
        BYTE    _mode        = 0x00; //is a bitmask that specifies each GPIO pin’s mode.
        BYTE    _function    = 0x00; //is a bitmask that specifies the special behavior of GPIO.0, GPIO.1, and GPIO.7
        BYTE    _m_clkDiv    = 0x00; //is the clock output divider value used for GPIO.7 when configured in clock output mode.

        status = HidSmbus_GetGpioConfig(*device, &_direction, &_mode, &_function, &_m_clkDiv);
        //Если конфиг передался с ошибкой
        if (status      != HID_SMBUS_SUCCESS    ||
            _direction  != direction            ||
            _mode       != mode                 ||
            _function   != function             ||
            _m_clkDiv   != m_clkDiv             )
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }

        //Прочитаем спецификацию CP2112
        HID_SMBUS_DEVICE_STR serial={0};
        HID_SMBUS_DEVICE_STR manufacturer={0};
        HID_SMBUS_DEVICE_STR product={0};
        status = HidSmbus_GetOpenedString(*device,serial,HID_SMBUS_GET_SERIAL_STR);
        if (status      != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }
        status = HidSmbus_GetOpenedString(*device,manufacturer,HID_SMBUS_GET_MANUFACTURER_STR);
        if (status      != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }
        status = HidSmbus_GetOpenedString(*device,product,HID_SMBUS_GET_PRODUCT_STR);
        if (status      != HID_SMBUS_SUCCESS)
        {
            cond[CONNECTED]=CP2112_Error_Handler(status);
            status = HidSmbus_Close(*device);
            cond[CONNECTED].append(" | Closing status is " + CP2112_Error_Handler(status));
            return STATUS_FAIL;
        }

        //Подготовим строку с параметрами
        str = "CP2112 connected";
        cond[CONNECTED]=str;
        str = "s/n: " + QString::fromStdString(serial);
        cond[SERIAL]=str;
        str = "Manufacturer: " + QString::fromStdString(manufacturer);
        cond[MANUFACTURER]=str;
        str = "Product: " + QString::fromStdString(product);
        cond[PRODUCT]=str;
        str = "SMBus Speed: " + QString::number(bitRate) + "Hz";
        cond[SPEED]=str;

        return STATUS_OK;
}

quint8 CP2112_Close(HID_SMBUS_DEVICE device, QString *str)
{
    HID_SMBUS_STATUS status = HidSmbus_Close(device);
    if (status != HID_SMBUS_SUCCESS)
    {
        str->append(" | Device is closed with status: ");
        str->append(CP2112_Error_Handler(status));
        return STATUS_FAIL;
    }
    return STATUS_OK;
}

QString CP2112_Error_Handler (HID_SMBUS_STATUS status)
{
    QString statusString;
    switch (status)
    {
    case HID_SMBUS_SUCCESS:
        statusString = "Success";
        break;
    case HID_SMBUS_DEVICE_NOT_FOUND:
        statusString = "Device not found";
        break;
    case HID_SMBUS_INVALID_HANDLE:
        statusString = "Invalid handle";
        break;
    case HID_SMBUS_INVALID_DEVICE_OBJECT:
        statusString = "Invalid device object";
        break;
    case HID_SMBUS_INVALID_PARAMETER:
        statusString = "Invalid parameter";
        break;
    case HID_SMBUS_INVALID_REQUEST_LENGTH:
        statusString = "Invalid request length";
        break;
    case HID_SMBUS_READ_ERROR:
        statusString = "Read error";
        break;
    case HID_SMBUS_WRITE_ERROR:
        statusString = "Write error";
        break;
    case HID_SMBUS_READ_TIMED_OUT:
        statusString = "Read timeout";
        break;
    case HID_SMBUS_WRITE_TIMED_OUT:
        statusString = "Write timeout";
        break;
    case HID_SMBUS_DEVICE_IO_FAILED:
        statusString = "Device IO failed";
        break;
    case HID_SMBUS_DEVICE_ACCESS_ERROR:
        statusString = "Device access error";
        break;
    case HID_SMBUS_DEVICE_NOT_SUPPORTED:
        statusString = "Device not supported";
        break;
    case HID_SMBUS_UNKNOWN_ERROR:
        statusString = "Unknown error";
        break;
    }
    return statusString;
}

void CP2112_Clear_Condition(QString cond[])
{
    for(quint8 i=0; i<6; i++)
    {
        cond[i].clear();
    }
}

void readINI ()
{
    QString configStr[] = {
        "BITRATE_HZ=",
        "ACK_ADDRESS=",
        "AUTO_RESPOND=",
        "WRITE_TIMEOUT_MS=",
        "READ_TIMEOUT_MS=",
        "TRANSFER_RETRIES=",
        "SCL_LOW_TIMEOUT=",
        "RESPONSE_TIMEOUT_MS=",
        "RX_TX_LEDS_ON=",
    };
    QString end = "\r\n";

    QFile file;
    file.setFileName("config.ini");
    if(!file.exists())//Если файл не существует
    {
        file.open(QIODevice::NewOnly); //Создадим новый файл
        QByteArray array = {0};
        QString str;

        str = configStr[Bitrate] + QString("%1").arg(BITRATE_HZ)+ end;
        array.append(str.toUtf8());
        str = configStr[ACK_Address] + QString("0x%1").arg(ACK_ADDRESS,2,16,QLatin1Char('0'))+ end;
        array.append(str.toUtf8());
        str = configStr[Auto_Respond] + QString(AUTO_RESPOND ? "true" : "false") + end;
        array.append(str.toUtf8());
        str = configStr[Write_Timeout] + QString("%1").arg(WRITE_TIMEOUT_MS)+ end;
        array.append(str.toUtf8());
        str = configStr[Read_Timeout] + QString("%1").arg(READ_TIMEOUT_MS)+ end;
        array.append(str.toUtf8());
        str = configStr[Transfer_Retries] + QString("%1").arg(TRANSFER_RETRIES)+ end;
        array.append(str.toUtf8());
        str = configStr[SCL_Low_Timeout] + QString(SCL_LOW_TIMEOUT ? "true" : "false") + end;
        array.append(str.toUtf8());
        str = configStr[Response_Timeout] + QString("%1").arg(RESPONSE_TIMEOUT_MS)+ end;
        array.append(str.toUtf8());
        str = configStr[RX_TX_Leds_On] + QString(RX_TX_LEDS_ON ? "true" : "false") + end;
        array.append(str.toUtf8());

        file.write(array);
        file.close();

        for(quint8 i=0; i<numcp2112config; i++)
        {
            cp2112config[i].userData = cp2112config[i].defaultData;
        }
        return;
    }
    if (!file.open(QFile::ReadOnly)) //проверяем открыт ли для чтения
    {
        return;
    }
    else
    {
        while (!file.atEnd()) {
            bool ok=false;
            QString line = file.readLine();
            for(quint8 i=0; i<numcp2112config; i++){
                if(line.contains(configStr[i], Qt::CaseSensitive)){
                    cp2112config[i].userData = getDataFromString (line, &ok);
                    if(!ok) cp2112config[i].userData = cp2112config[i].defaultData;
                }
            }
        }
        file.close();                   //Закрываем файл
        return;
    }
}

QVariant getDataFromString (QString str, bool *ok)
{
    str = str.section('=', 1,1);//Отделим все до знака "="
    str = str.section('\r',0,0); //Оставим только то, что перед \r
    quint8 strLen = str.size();
    if(strLen == 0)
    {
        *ok=false; //Если строка пустая, выходим
        return 0;
    }
    if (str == "true") return true;
    if (str == "false") return false;
    quint32 temp = str.toUInt(ok);
    return temp;
}

void writeINI ()
{
    QString configStr[] = {
        "BITRATE_HZ=",
        "ACK_ADDRESS=",
        "AUTO_RESPOND=",
        "WRITE_TIMEOUT_MS=",
        "READ_TIMEOUT_MS=",
        "TRANSFER_RETRIES=",
        "SCL_LOW_TIMEOUT=",
        "RESPONSE_TIMEOUT_MS=",
        "RX_TX_LEDS_ON=",
    };
    QString end = "\r\n";

    QFile file;
    file.setFileName("config.ini");
    if(!file.exists())return;//Если файл не существует

    file.open(QIODevice::WriteOnly); //Если файл доступен для записи
    QByteArray array = {0};
    QString str;

    str = configStr[Bitrate] + QString("%1").arg(cp2112config[Bitrate].userData.toInt())+ end;
    array.append(str.toUtf8());
    str = configStr[ACK_Address] + QString("0x%1").arg(cp2112config[ACK_Address].userData.toInt(),2,16,QLatin1Char('0'))+ end;
    array.append(str.toUtf8());
    str = configStr[Auto_Respond] + QString(cp2112config[ACK_Address].userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());
    str = configStr[Write_Timeout] + QString("%1").arg(cp2112config[Write_Timeout].userData.toInt())+ end;
    array.append(str.toUtf8());
    str = configStr[Read_Timeout] + QString("%1").arg(cp2112config[Read_Timeout].userData.toInt())+ end;
    array.append(str.toUtf8());
    str = configStr[Transfer_Retries] + QString("%1").arg(cp2112config[Transfer_Retries].userData.toInt())+ end;
    array.append(str.toUtf8());
    str = configStr[SCL_Low_Timeout] + QString(cp2112config[SCL_Low_Timeout].userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());
    str = configStr[Response_Timeout] + QString("%1").arg(cp2112config[Response_Timeout].userData.toInt())+ end;
    array.append(str.toUtf8());
    str = configStr[RX_TX_Leds_On] + QString(cp2112config[RX_TX_Leds_On].userData.toBool() ? "true" : "false") + end;
    array.append(str.toUtf8());

    file.write(array);
    file.close();
}
