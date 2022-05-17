#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cp2112.h"
#include "BatteryCommands.h"
#include "sh366000_addresses.h"
#include "qdebug.h"
//#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QIODevice>
#include <string.h> //Без этого не работает memcpy
//#include <Qt>
#include <QStandardItemModel>
#include <QByteArray>
#include <QLabel>
#include <QProgressBar>
#include <QCloseEvent>

HID_SMBUS_DEVICE m_hidSmbus         = 0; //Адрес устройства CP2112 с точки зрения винды
STATUS HIDstatus                    = STATUS_OK;
Array flashDataArray                = EMPTY;
QByteArray flashData                = {0};
QString cp2112condition [7]={0};
enum calibData {temperature, voltage, current};

#define DATA_COLUMN 2

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CP2112 SH366000 Flasher");
    ui->actionSave->setIcon(QIcon(":/icons/save256.ico"));
    ui->actionOpen->setIcon(QIcon(":/icons/openfile256.ico"));
    ui->actionSettings->setIcon(QIcon(":/icons/configure.ico"));
    ui->actionInfo->setIcon(QIcon(":/icons/info.ico"));
    ui->readFlash->setIcon(QIcon(":/icons/datadownload.ico"));
    ui->readSMB->setIcon(QIcon(":/icons/datadownload.ico"));
    ui->readBits->setIcon(QIcon(":/icons/datadownload.ico"));
    ui->writeFlash->setIcon(QIcon(":/icons/dataupload.ico"));

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    timer->start(2000); // Запустим таймер на 2 секунды
    calibTimer = new QTimer();
    calibTimer->start(500); // Запустим таймер вкладки калибровка на 1 сек

    statusbarTable = new QTableWidget(this); //Создадим пустой виджет таблицы
    ui->statusbar->addPermanentWidget(statusbarTable);//Установим его в статусбар
    setStatusbarTable(statusbarTable);//Функция для настройки таблицы

    setTable(ui->tableWidget);
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));

    setSBSTable (ui->tableWidget_2);

    quint8 bitTableSize;
//    QVBoxLayout *lay = new QVBoxLayout(this);   //Создадим слой VBox

    QVBoxLayout *lay = ui->verticalLayout_2;

    tableBatteryMode    = new QTableWidget(this);
    tableBatteryStatus  = new QTableWidget(this);
    tablePackStatus     = new QTableWidget(this);   //Создадим виджеты таблиц
    tablePackConfig     = new QTableWidget(this);
    tableAFE_Status     = new QTableWidget(this);
    tableSsafe          = new QTableWidget(this);

    QLabel *labelBatteryMode        = new QLabel("(0x03) Battery Mode", this);
    QLabel *labelBatteryStatus      = new QLabel("(0x16) Battery Status", this);
    QLabel *labelNoteBatteryStatus  = new QLabel("Note*: Bits 0-3 means: 0000 - OK, 0111 - Error");
    QLabel *labelPackStatus         = new QLabel("(0x2F LSB) Pack Status", this);
    QLabel *labelPackConfig         = new QLabel("(0x2F MSB) Pack Config", this);
    QLabel *labelNotePackConfig     = new QLabel("Note*: Bits 11-12 means: 00 or 01 - 2cell, 10 - 3cell, 11 - 4cell", this);
    QLabel *labelAFE_Status         = new QLabel("(0x46) AFE Status", this);
    QLabel *labelSsafe              = new QLabel("(0x47) Ssafe", this);

    lay->addWidget(labelBatteryMode);
    lay->addWidget(tableBatteryMode);

    lay->addWidget(labelBatteryStatus);
    lay->addWidget(tableBatteryStatus);
    lay->addWidget(labelNoteBatteryStatus);

    lay->addWidget(labelPackStatus);
    lay->addWidget(tablePackStatus);

    lay->addWidget(labelPackConfig);//Разместим таблицы на слое
    lay->addWidget(tablePackConfig);
    lay->addWidget(labelNotePackConfig);

    lay->addWidget(labelAFE_Status);
    lay->addWidget(tableAFE_Status);

    lay->addWidget(labelSsafe);
    lay->addWidget(tableSsafe);

    bitTableSize = sizeof(BatteryMode)/sizeof(BatteryMode[0]);  //Определим количество строк
    setBinTable(tableBatteryMode, BatteryMode, bitTableSize);    //Настроим таблицы

    bitTableSize = sizeof(BatteryStatus)/sizeof(BatteryStatus[0]);  //Определим количество строк
    setBinTable(tableBatteryStatus, BatteryStatus, bitTableSize);    //Настроим таблицы

    bitTableSize = sizeof(Pack_Status)/sizeof(Pack_Status[0]);  //Определим количество строк
    setBinTable(tablePackStatus, Pack_Status, bitTableSize);    //Настроим таблицы

    bitTableSize = sizeof(Pack_Config)/sizeof(Pack_Config[0]);
    setBinTable(tablePackConfig, Pack_Config, bitTableSize);

    bitTableSize = sizeof(AFE_Status)/sizeof(AFE_Status[0]);
    setBinTable(tableAFE_Status, AFE_Status, bitTableSize);

    bitTableSize = sizeof(Ssafe)/sizeof(Ssafe[0]);
    setBinTable(tableSsafe, Ssafe, bitTableSize);

    ui->scrollAreaWidgetContents_2->setLayout(lay); //Установим слой на область прокрутки

    quint8 index = ui->tabWidget->indexOf(ui->tab_1); //Найдем индекс первой вкладки
    ui->tabWidget->setCurrentIndex(index);  //Установим ее как текущую

}

MainWindow::~MainWindow()
{
//    configWindowUI.close();
    delete ui;
}


void MainWindow::slotTimerAlarm()
/*Конфигурирует CP2112, отображает о нем информацию*/
{
    if(HIDstatus == TRANSFER_IN_PROGRESS)return; //Если идет передача данных, выходим
    HIDstatus = TRANSFER_IN_PROGRESS;
    STATUS status;
    status = CP2112_Init(&m_hidSmbus, cp2112condition);//Конфигурируем CP2112 только чтобы отобразить о нем информацию
    if(status == STATUS_OK)
    {
        qint32 data=0;
        if (!ReadWord(m_hidSmbus, &data, 0x0d))
        {
            cp2112condition[5]=QString("Battery Not Connected");
        }
        else
        {
            cp2112condition[5]=QString("Battery Connected");
        }
        cp2112condition[6]=QString::number(data);
    }

    fillStatusbarTable(statusbarTable, cp2112condition);//Заполняем статусбар
    transferStop();
}

void MainWindow::on_readFlash_clicked()
{
    if(!transferStart())return;

    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
    flashDataArray  = EMPTY;

    quint8   num    = sizeof(flash)/sizeof(flash[0]); //Вычисляем количество элементов в массиве с адресами
    quint16  size   = flash[num-1].regAddr + flash[num-1].dataLen;  //Вычисляем максимальный размер массива для данных из флеш
    //Это последний адрес+длинна слова
    flashData.resize(size); //Изменяем размер массива для данных из флеш до нужного размера
    flashData.fill(0);  //Заполняем массив нулями

    for (quint8 i=0; i<num; i++)
    {
        quint8 buffer [16]={0}; //Создаем приемный буфер

        ReadFlash(m_hidSmbus,flash[i].regAddr,flash[i].dataLen,buffer); //Читаем флеш

        switch (flash[i].dataLen)
        {
        case 0x01: //Если длина данных один байт
            flashData[flash[i].regAddr]=buffer[0]; //Заносим в массив значение
            break;

        case 0x02: //Если длина данных два байта
            flashData[flash[i].regAddr]=buffer[0];                //Заносим в массив значения
            flashData[flash[i].regAddr+1]=buffer[1];              //Заносим в массив значения
            break;

        case 0x10: //Если строка
            //Копируем строку+1 байт(содержит длину строки) в массив, начиная с flashData.data()+flash[i].regAddr
            memcpy(flashData.data()+flash[i].regAddr, buffer, buffer[0]+1);//Копируем строку+1 байт(содержит длину строки) в массив
            break;
        }
    }
    flashDataArray = READY;
    readDataFromArray();
    transferStop();
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
}

void MainWindow::on_writeFlash_clicked()
{
    if(!transferStart())return;

    if (flashDataArray == EMPTY) //если массив пустой, выходим
    {
        QMessageBox::critical(this,"Error","There is nothing to write to flash,\nthe data array is empty!");
        HIDstatus = TRANSFER_COMPLETED; //Ставим флаг, что передача данных закончена
        return;
    }

    quint8 num = sizeof(flash)/sizeof(flash[0]); //Вычисляем количество элементов в массиве с адресами
    for (quint8 i=0; i<num; i++)
    {
        quint8 dat_len = 0;//размер данных, передаваемых за раз
        quint8 dat[0x10];//массив с данными, 0x10 - максимальный размер данных
        switch (flash[i].dataLen)
        {
        case 0x01: //Если длина данных один байт
        {
            dat_len = 0x01;
            dat [0] = flashData[flash[i].regAddr];
            break;
        }
        case 0x02: //Если длина данных два байта
        {
            dat_len = 0x02;
            dat [0] = (quint8)flashData[flash[i].regAddr];
            dat [1] = (quint8)flashData[flash[i].regAddr+1];
            break;
        }
        case 0x10: //Если строка
        {
            dat_len = flashData[flash[i].regAddr]+1;//плюс 1 байт с размером строки
            quint16 dat_start  = flash[i].regAddr;
            for (quint8 n=0; n<dat_len; n++)
            {
                dat [n] = flashData[dat_start+n];
            }
            break;
        }
        }
        if (!WriteFlash(m_hidSmbus, flash[i].regAddr, dat_len, dat))
        {
            QMessageBox::critical(this,"Error","Write error!");
            HIDstatus = TRANSFER_COMPLETED; //Ставим флаг, что передача данных закончена
            return;
        }
    }
    transferStop();
}

void MainWindow::on_actionSave_triggered()
{
    if (flashDataArray != READY)
    {
        QMessageBox::critical(this,"Error","Flash must be read first!\nOr read data from a file");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    QDir::homePath(),
                                                    tr("Binary File(*.bin)") );

    if(fileName.isNull())
    {
        QMessageBox::information(this,"Error","File not selected");
    }
    else
    {
        if(saveToFile(&fileName, &flashData)!=STATUS_OK) //Сохраняем в файл
        {
            QMessageBox::information(this,"Error","File cannot be opened");
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open file",
                                                    QDir::homePath(),
                                                    tr("Only binary files(*.bin)"));
    if(fileName.isNull())
    {
        QMessageBox::information(this,"Error","File not selected");
    }
    else
    {
        quint8   num    = sizeof(flash)/sizeof(flash[0]); //Вычисляем количество элементов в массиве с адресами
        quint16  size   = flash[num-1].regAddr + flash[num-1].dataLen;  //Вычисляем максимальный размер массива для данных из флеш
        //Это последний адрес+длинна слова
        flashData.resize(size); //Изменяем размер массива для данных из флеш до нужного размера
        flashData.fill(0);  //Заполняем массив нулями
        flashDataArray  = EMPTY; //Ставим флаг, что массив теперь пустой
        if(readFromFile(&fileName, &flashData)==STATUS_OK)
        {
            flashDataArray  = READY; //Ставим флаг, что массив готов
            readDataFromArray();
        }
        else
        {
            QMessageBox::information(this,"Error","File cannot be opened");
        }
    }
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int)));
}

void MainWindow::setTable (QTableWidget *table)
{
    //Пустая таблица должна быть создана в редакторе виджетов, здесь настраиваем
    quint8 rows = sizeof(flash)/sizeof(flash[0]);

    table->setRowCount(rows);       //число строк
    table->setColumnCount(4);       //число столбцов
    table->setColumnWidth(0,80);    //Номер регистра
    table->setColumnWidth(1,220);   //Наименование параметра
    table->setColumnWidth(2,120);   //Значение параметра
    table->setColumnWidth(3,50);    //Единица измерения
    table->horizontalHeader()->setStretchLastSection(true); // Устанавливаем адаптивный размер последнего столбца
    table->verticalHeader()->setMinimumSectionSize(1); //Без этого невозможно уменьшить высоту ячейки меньше определенного

    for(uint8_t i=0; i<rows; i++)
    {
        table->setRowHeight(i,18); //Высота ячеек
    }

    QStringList horizHeaders;                               //Объявим список,
    horizHeaders<<"Reg Address"<<"Name"<<"Data"<<"Unit";    //в который положим имена столбцов
    table->setHorizontalHeaderLabels(horizHeaders);         //отобразим имена столбцов

    table->verticalHeader()->hide(); //Имена строк наоборот, спрячем

    table->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(240, 240, 240);}"); //Цвет фона заголовка
//    table->setEditTriggers(QTableWidget::NoEditTriggers); //Запрет редактирования таблицы

    for (uint8_t i=0; i<rows ; i++)    //Заполним столбцы
    {
        QTableWidgetItem *itm = new QTableWidgetItem(QString("%1")          //Строка с одним аргументом
                                                     .arg(flash[i].regAddr, //Аргумент - номер регистра
                                                          4,                //Минимум 4 символа
                                                          16,               //Шестнадцатиричных
                                                          QLatin1Char('0')) //Ели не хватает символов, заполнить нулями
                                                     .toUpper()             //Все символы в верхнем регистре;
                                                     .prepend("0x"));       //Добавить перед строкой

        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,0,itm);                                //Устанавливаем данные в первый столбец
        table->item(i,0)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        itm = new QTableWidgetItem(QString(flash[i].name));
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,1,itm);                                //Устанавливаем данные во второй столбец

        itm = new QTableWidgetItem("");
        itm->setBackground(QColor(255,255,255) );               //Задний фон белый
        table->setItem(i,2,itm);                                //Устанавливаем данные в третий столбец
        table->item(i,2)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        itm = new QTableWidgetItem(QString(flash[i].unit));
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,3,itm);                                //Устанавливаем данные в четвертый столбец
        table->item(i,3)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру
//        table->item(i,1)->setBackground(Qt::GlobalColor(Qt::white));
    }
}

//Чтение флешки по 16 байт за раз, оставляю алгоритм на всякий случай
bool MainWindow::readBatteryFlash(HID_SMBUS_DEVICE &device, QByteArray &flashData)
{
    const BYTE numOfRead        = 0x7f; //Сколько раз нужно прочитать
    const BYTE startAddress     = 0x00; //Начальный адрес для чтения
    const BYTE sizeOfBuff       = 0x10; //Размер буфера чтения

    for (BYTE i=0; i<numOfRead; i++) //Читаем флешку numOfRead раз
    {
        BYTE data [sizeOfBuff]={0}; //буфером по sizeOfBuff байт

        if(ReadFlash(device,startAddress+i*sizeOfBuff,sizeOfBuff,data)==STATUS_OK) //за раз соответственно получаем sizeOfBuff байт
        {
            flashData.resize(sizeOfBuff+i*sizeOfBuff); //Увеличиваем размер приемного массива на величину прочитаных байт
            memcpy(flashData.data()+i*sizeOfBuff, data, sizeOfBuff); //Добавляем данные в конец к существующим

            //       //Еще один способ передачи данных в массив QByteArray, побайтно
            //       for (int n = 0; n < sizeOfBuff; ++n) //складываем в массив
            //        {
            //            flashRAWdata.append((const char*)(data + n), sizeof(uint8_t));
            //        }
        }
        else
        {
            return STATUS_FAIL;
        }

    }
    return STATUS_OK;
}

bool MainWindow::saveToFile(QString *pathToFile, QByteArray *dataToSave)
{
    //Создаем или открываем файл
//    QFile file(*pathToFile); //или по другому
    QFile file;
    file.setFileName(*pathToFile);

    if (!file.open(QFile::WriteOnly)) //проверяем открыт ли для записи
    {
        return STATUS_FAIL;
    }
    else
    {
        file.write(*dataToSave);
        file.close();
        return STATUS_OK;
    }
}

bool MainWindow::readFromFile(QString *pathToFile, QByteArray *dataFromFile)
{
    //Создаем или открываем файл
//    QFile file(*pathToFile); //или по другому
    QFile file;
    file.setFileName(*pathToFile);

    if (!file.open(QFile::ReadOnly)) //проверяем открыт ли для чтения
    {
        return STATUS_FAIL;
    }
    else
    {
        *dataFromFile = file.readAll(); //Читаем из файла в массив
        file.close();                   //Закрываем файл

        return STATUS_OK;
    }
}

void MainWindow::readDataFromArray(void)
{
    if (flashDataArray == EMPTY) //если массив пустой, выходим
    {
        return;
    }
    quint8 num = sizeof(flash)/sizeof(flash[0]); //Вычисляем количество элементов в массиве с адресами
    QString str;

    for (quint8 i=0; i<num; i++)
    {
        switch (flash[i].dataLen)
        {
        case 0x01: //Если длина данных один байт
        {
            quint8 dat = flashData[flash[i].regAddr];   //Переносим данные во временную переменную
            if(flash[i].unit == "HEX")
            {
                str = QString ("%1").arg (dat,2,16,QLatin1Char('0')).toUpper();   //Минимальная длина 2, 16-тичная система
//                str.prepend("0x");
            }
            else
            {
                str = QString ("%1").arg (dat,1,10);        //Минимальная длина 1, 10-тичная система
            }

            ui->tableWidget->item(i,DATA_COLUMN)->setText(str);

            break;
        }
        case 0x02: //Если длина данных два байта
        {
            quint8 highByte   = (BYTE)flashData[flash[i].regAddr];
            quint8 lowByte    = (BYTE)flashData[flash[i].regAddr+1];
            qint32 dat;
            if(flash[i].unsign)//Если число беззнаковое
            {
                quint16 datUINT = (highByte<<8|lowByte); //Склеиваем два байта в слово
                dat = datUINT;
            }
            else//Если число со знаком
            {
                qint16 datINT = (highByte<<8|lowByte); //Склеиваем два байта в слово
                dat = (qint32)datINT;
            }

            if(flash[i].unit == "HEX")
            {
                str = QString ("%1").arg (dat,4,16,QLatin1Char('0')).toUpper();   //Минимальная длина 2, 16-тичная система
//                str.prepend("0x");
            }
            else if (flash[i].unit == "d-m-y")
            {
                QDate date; //Объявляем переменную date
                quint16 day = 0b00011111 & dat;
                quint16 month = dat >> 5 & 0b00001111;
                quint16 year = 1980 + (dat >> 9 & 0b01111111);
                date.setDate(year, month, day); //Инициализируем переменную date полученными значениями
                str = date.toString("dd-MMM-yyyy"); //Преобразование с форматированием даты
            }
            else if ((flash[i].unit == "°C"))
            {
                float tempC = dat/10. - 273.1;
                str = QString::number(tempC);
            }
            else if ((flash[i].unit == "mOhm"))
            {
                float res = dat/100.;
                str = QString::number(res);
            }
            else
            {
                str = QString("%1").arg(dat,1,10);  //Минимальная длина 1, десятичная система
            }
            ui->tableWidget->item(i,DATA_COLUMN)->setText(str);
            break;
        }
        case 0x10: //Если строка
        {
            qsizetype str_len      = flashData[flash[i].regAddr];  //Узнаем длину строки
            quint16 str_start    = flash[i].regAddr+1;           //Начало строки
//            QChar dataChar [16]; //Создаем массив QChar

//            for (BYTE n=0; n<str_len; n++)
//            {
//                dataChar[n] = flashData[str_start+n]; //Копируем побайтно
//            }
//            QString str(dataChar, str_len);
            //Из массива символов создаем строку QString
            str = QString::fromLocal8Bit(&flashData[str_start], str_len);
            ui->tableWidget->item(i,DATA_COLUMN)->setText(str);
            break;
        }
        }
    }
}

void MainWindow::replaceData(int row, int column)
{
   QString str; //Объявляем строку для приема текста
   QTableWidgetItem *itm = ui->tableWidget->item(row,column); //Объявим указатель на предмет(ячейку)
   if (NULL != itm) { //Если указатель не нулевой
      str = itm->text(); //Получаем строку из ячейки
   }
   if(flashDataArray  != READY) //Если буфер пустой
   {
       QMessageBox::critical(this,"Error","Read battery first or load data from file!");
       clearItem(row, column);
       return;
   }
   quint8 len = flash[row].dataLen; //Определим длину ожидаемых данных
   bool ok = 0; //Флаг успешного преобразования из текста в число
   qint32 dat=0; //Переменная для полученного числа
   switch (len)
   {
   case 0x01: //Если число однобайтное
       if(flash[row].unit == "HEX")
       {
           dat = str.toUInt(&ok, 16); //Преобразуем строку в число
       }
       else
       {
           dat = str.toUInt(&ok, 10); //Преобразуем строку в число
       }

       if (!ok || dat>0xff) //Если преобразование не успешное или число больше чем 0xff
       {
           QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between 0 and 255\nor 0x00 and 0xFF");
           clearItem(row, column);
           return;
       }
       flashData[flash[row].regAddr]=dat; //Если все успешно, занесем данные в массив
       break;
   case 0x02: //Если двухбайтное число
       if(flash[row].unit == "HEX")
       {
           dat = str.toUInt(&ok, 16); //Преобразуем строку в число
           if (dat >0xffff) ok = false;
       }
       else if (flash[row].unit == "°C")
       {
           float temp = str.toFloat(&ok);
           temp = (temp + 273.1)*10.0;
           dat = temp;
           if (dat >0xffff) ok = false;
       }
       else if (flash[row].unit == "d-m-y")
       {
           qsizetype strSize = str.size(); //Вычисляем длину строки
           if(strSize >10)
           {
               QMessageBox::critical(this,"Error","Incorrect date entered\nEnter the date in the format: dd-mm-yyyy");
               clearItem(row, column);
               return;
           }
           QString dd,mm,yyyy;
           quint8 day,month;
           quint16 year;
           dd =     str.at(0);
           dd +=    str.at(1);
           mm =     str.at(3);
           mm +=    str.at(4);
           yyyy =   str.at(6);
           yyyy +=  str.at(7);
           yyyy +=  str.at(8);
           yyyy +=  str.at(9);
           day = dd.toUInt(&ok, 10);        //Преобразуем строку в число
           if (!ok || day>31 || day<1)      //Если преобразование не успешное или день неправильный
           {
               QMessageBox::critical(this,"Error","Incorrect day entered\nDay cannot be more than 31\nor less than 1");
               clearItem(row, column);
               return;
           }
           month = mm.toUInt(&ok, 10);      //Преобразуем строку в число
           if (!ok || month>12 || month<1)  //Если преобразование не успешное или месяц неправильный
           {
               QMessageBox::critical(this,"Error","Incorrect month entered\nMonth cannot be more than 12\nor less than 1");
               clearItem(row, column);
               return;
           }
           year = yyyy.toUInt(&ok, 10);     //Преобразуем строку в число
           if (!ok || year>2100 || year<1980) //Если преобразование не успешное или год неправильный
           {
               QMessageBox::critical(this,"Error","Incorrect year entered\nYear cannot be more than 2100\nor less than 1980");
               clearItem(row, column);
               return;
           }
           dat = (year-1980)*512+month*32+day;
       }
       else if (flash[row].unit == "mOhm")
       {
           float mOhm = str.toFloat(&ok);
           mOhm = mOhm*100.0;
           dat = mOhm;
           if (dat >0xffff) ok = false;
       }
       else if (flash[row].unsign == 0)//если число со знаком
       {
           dat = str.toInt(&ok, 10); //Преобразуем строку в число
           if (dat >32767 || dat < -32767) ok = false;
       }
       else if (flash[row].unsign == 1)//если число без знака
       {
           dat = str.toUInt(&ok, 10); //Преобразуем строку в число
       }

       if (!ok || dat>0xffff) //Если преобразование не успешное или число больше 0xffff
       {
           QMessageBox::critical(this,"Error","Incorrect data entered");
           clearItem(row, column);
           return;
       }
       flashData[flash[row].regAddr]=dat>>8; //Если все успешно, заносим данные в массив
       flashData[flash[row].regAddr+1]=dat&0xff;
       break;
   case 0x10: //Если строка
   {
          qsizetype strSize = str.size(); //Вычисляем длину строки
          if (strSize>15) //Проверяем допустима ли длина
          {
              QMessageBox::critical(this,"Error","Incorrect data entered.\nThe string must be no longer than 15 characters!");
              clearItem(row, column);
              return;
          }
          for (BYTE i=0; i<strSize; i++)
          {
              QChar ch = str.at(i); //Достаем отдельный символ из строки
              if (ch.unicode() > 127) //Проверяем допустимый ли символ
              {
                  QMessageBox::critical(this,"Error","Incorrect data entered.\nThe string cannot contain non-latin characters!");
                  clearItem(row, column);
                  return;
              }
          }
          flashData[flash[row].regAddr]=strSize; //Если все успешно, заносим данные в массив заносим в ячейку длину строки
          for (BYTE i=0; i<strSize; i++)
          {
              flashData[flash[row].regAddr+1+i]=str[i].toLatin1(); //Потом символы по одному
          }
   }
       break;
   }
   flashDataArray = READY;
   return;
}

void MainWindow::clearItem(int row, int column)
{
    disconnect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int))); //Отключим сигнал
    ui->tableWidget->item(row,column)->setText(""); //Удалим данные из ячейки
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(replaceData(int,int))); //Включим сигнал
//    flashDataArray = EMPTY; //Поднимем флаг, что массив пустой
    return;
}

void MainWindow::setStatusbarTable(QTableWidget *table)
{
    quint8 rows     = 2;
    quint8 columns  = 4;
    table->setRowCount(rows);       //число строк
    table->setColumnCount(columns); //число столбцов
    table->setColumnWidth(0,130);
    table->setColumnWidth(1,250);
    table->setColumnWidth(2,160);
    table->setColumnWidth(3,60);
    table->horizontalHeader()->setStretchLastSection(true); // Устанавливаем адаптивный размер последнего столбца
    table->verticalHeader()->setMinimumSectionSize(1); //Без этого невозможно уменьшить высоту ячейки меньше определенного
    table->setRowHeight(0,18); //Высота ячеек
    table->setRowHeight(1,18); //Высота ячеек
    table->setFixedSize(656,38);
    table->horizontalHeader()->hide(); //скроем заголовки
    table->verticalHeader()->hide();   //скроем заголовки

    for(quint8 x=0; x<rows; x++)
    {
        for(quint8 y=0; y<columns; y++)
        {
            QTableWidgetItem *itm = new QTableWidgetItem();         //Создадим пустой предмет таблицы
            table->setItem(x,y,itm);                                //установим в каждую ячейку
            table->item(x,y)->setBackground(QColor(240,240,240));   //Задний фон
            table->item(x,y)->setFlags(Qt::ItemIsEnabled);//Включим только один флаг
//            table->item(x,y)->setFlags(Qt::NoItemFlags);
        }
    }
}

void MainWindow::fillStatusbarTable(QTableWidget *table, QString cond[])
{
    table->item(0,0)->setText(cond[0]);
    table->item(1,0)->setText(cond[1]);
    table->item(0,1)->setText(cond[2]);
    table->item(1,1)->setText(cond[3]);
    table->item(0,2)->setText(cond[4]);
    table->item(1,2)->setText(cond[5]);
    QProgressBar *batteryLevel = new QProgressBar();
    table->setCellWidget(0,3,batteryLevel);
    batteryLevel->setRange(0,100);
//    bool ok;
    quint8 battLevel = cond[6].toUInt();
    batteryLevel->setValue(battLevel);
}

void MainWindow::send_command (Commands command)
{
    if(!transferStart())return;

    QString str = QString("%1")     //Строка, в которой будет один аргумент
            .arg(command,           //А вот и аргумент
                 4,                 //Четыре символа
                 16,                //HEX
                 QLatin1Char('0'))  //C лидирующими нулями
            .toUpper();             //Все символы в верхнем регистре
    str = str.prepend("0x");        //Добавим перед строкой
    ui->CommandID->setText(str);
    ui->CommandID->setAlignment(Qt::AlignCenter);

    if(WriteWord(m_hidSmbus, command, 0x00))
    {
        qint32 data=0;
        ReadWord(m_hidSmbus,&data,0x00);
        ui->Details->setText(QString::number(data));
    }
    else
    {
        ui->Details->setText("Error!");
    }
    ui->Details->setAlignment(Qt::AlignCenter);

    transferStop();
}

void MainWindow::on_DeviceType_clicked()
{
    send_command (DEVICE_TYPE);
}

void MainWindow::on_FirmwareRev_clicked()
{
    send_command (FIRMVARE_REV);
}

void MainWindow::on_EDVLevel_clicked()
{
    send_command (EDV_LEVEL);
}

void MainWindow::on_Shutdown_clicked()
{
    send_command (SHUTDOWN);
}

void MainWindow::on_Seal_clicked()
{
    send_command (SEAL);
}

void MainWindow::on_Reset_clicked()
{
    send_command (RESET);
}

void MainWindow::setSBSTable (QTableWidget *table)
{
    enum headers {addr, access, name, data, unit};
    //Пустая таблица должна быть создана в редакторе виджетов, здесь настраиваем
    quint8 rows = sizeof(Standard_Commands)/sizeof(Standard_Commands[0]);
    quint8 column = 5;

    table->setRowCount(rows);       //число строк
    table->setColumnCount(column);  //число столбцов
    table->setColumnWidth(addr,40);    //Номер регистра
    table->setColumnWidth(access,50);   //Наименование параметра
    table->setColumnWidth(name,150);   //Значение параметра
    table->setColumnWidth(data,150);    //Единица измерения
    table->setColumnWidth(unit,40);    //Единица измерения
    table->horizontalHeader()->setStretchLastSection(true); // Устанавливаем адаптивный размер последнего столбца
    table->verticalHeader()->setMinimumSectionSize(1); //Без этого невозможно уменьшить высоту ячейки меньше определенного

    for(uint8_t i=0; i<rows; i++)
    {
        table->setRowHeight(i,18); //Высота ячеек
    }

    QStringList horizHeaders;                               //Объявим список,
    horizHeaders<<"Reg"<<"Access"<<"Name"<<"Data"<<"Unit";    //в который положим имена столбцов
    table->setHorizontalHeaderLabels(horizHeaders);         //отобразим имена столбцов

    table->verticalHeader()->hide(); //Имена строк наоборот, спрячем

    table->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(240, 240, 240);}"); //Цвет фона заголовка
//    table->setEditTriggers(QTableWidget::NoEditTriggers); //Запрет редактирования таблицы

    for (uint8_t i=0; i<rows ; i++)    //Заполним столбцы
    {
        QTableWidgetItem *itm = new QTableWidgetItem(QString("%1")          //Строка с одним аргументом
                                                     .arg(Standard_Commands[i].addr, //Аргумент - номер регистра
                                                          2,                //Минимум 4 символа
                                                          16,               //Шестнадцатиричных
                                                          QLatin1Char('0')) //Ели не хватает символов, заполнить нулями
                                                     .toUpper()             //Все символы в верхнем регистре;
                                                     .prepend("0x"));       //Добавить перед строкой

        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,addr,itm);                             //Устанавливаем данные в первый столбец
        table->item(i,addr)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        QString accessStr;
        if (Standard_Commands[i].access==RW) accessStr = "RW";
        if (Standard_Commands[i].access==R) accessStr = "R";
        itm = new QTableWidgetItem(accessStr);
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,access,itm);                           //Устанавливаем данные во второй столбец
        table->item(i,access)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        itm = new QTableWidgetItem(QString(Standard_Commands[i].name));
        itm->setBackground(QColor(255,255,255));               //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,name,itm);                             //Устанавливаем данные в третий столбец

        itm = new QTableWidgetItem("");
        itm->setBackground(QColor(255,255,255) );               //Задний фон белый
        //Если только для чтения, снимаем все флаги, выставляя только один флаг
        if(Standard_Commands[i].access == R)itm->setFlags(Qt::ItemIsEnabled);
        table->setItem(i,data,itm);                             //Устанавливаем данные в третий столбец
        table->item(i,data)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        itm = new QTableWidgetItem(QString(Standard_Commands[i].unit));
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,unit,itm);                                //Устанавливаем данные в четвертый столбец
        table->item(i,unit)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру
    }
}

void MainWindow::on_readSMB_clicked()
{
    if(!transferStart())return;

    quint8   num    = sizeof(Standard_Commands)/sizeof(Standard_Commands[0]); //Вычисляем количество элементов в массиве с адресами

    for(quint8 i=0; i<num; i++)
    {
        qint32 dat=0;
        QString str="";

        if(Standard_Commands[i].unit == "HEX")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg(dat,4,16, QLatin1Char('0')).toUpper();
            str.prepend("0x");
        }
        else if(Standard_Commands[i].unit == "ASCII")
        {
            char dataChar[16]={0};
            qint32 len=0;
            ReadTextBlock(m_hidSmbus, dataChar, &len, Standard_Commands[i].addr);
            for(quint8 i=0; i<len; i++)
            {
                str.append(dataChar[i]);
            }
        }
        else if(Standard_Commands[i].unit == "Block")
        {
            quint8 dataBlock[32]={0};
            qint32 len=0;
            ReadDataBlock(m_hidSmbus, dataBlock, &len, Standard_Commands[i].addr);
            for(quint8 i=0; i<len; i++)
            {
                str.append(QString("%1 ").arg(dataBlock[i],2,16, QLatin1Char('0')).toUpper());
            }
        }
        else if(Standard_Commands[i].unit == "°C")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            float tempC = dat/10. - 273.1;
            str = QString::number(tempC);
        }
        else if (Standard_Commands[i].unit == "d-m-y")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            QDate date; //Объявляем переменную date
            quint16 day = 0b00011111 & dat;
            quint16 month = dat >> 5 & 0b00001111;
            quint16 year = 1980 + (dat >> 9 & 0b01111111);
            date.setDate(year, month, day); //Инициализируем переменную date полученными значениями
            str = date.toString("dd-MMM-yyyy"); //Преобразование с форматированием даты
        }
        else if(Standard_Commands[i].unit == "LBS")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg((quint8)dat & 0xff, 8, 2,QLatin1Char('0'));
            str.prepend("0b");
        }
        else if(Standard_Commands[i].unit == "MBS")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg(dat>>8, 8, 2,QLatin1Char('0'));
            str.prepend("0b");
        }
        else if(Standard_Commands[i].unit == "BIN")
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg(dat, 16, 2,QLatin1Char('0'));
            str.prepend("0b");
        }
        else if(Standard_Commands[i].unsign==I)
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg((qint16)dat,1,10);
        }
        else
        {
            ReadWord(m_hidSmbus, &dat, Standard_Commands[i].addr);
            str = QString("%1").arg(dat,1,10);
        }
        ui->tableWidget_2->item(i,SBS_DATA_COLUMN)->setText(str);
    }
    transferStop();
}

void MainWindow::setBinTable(QTableWidget *table, const struct bin_data *data, quint8 size)
{
    //Пустая таблица должна быть создана в редакторе виджетов, здесь настраиваем
    quint8  rows    = size;
    quint8  column  = 3;
    quint16 rowsH   = 18;
    quint8  headerH = 22;

    table->setRowCount(rows);       //число строк
    table->setColumnCount(column);  //число столбцов
    table->setColumnWidth(BIT_NUM,20);    //Номер регистра
    table->setColumnWidth(BIT_NAME,230);   //Наименование параметра
    table->setColumnWidth(BIT_STATUS,70);   //Значение параметра
    table->horizontalHeader()->setStretchLastSection(true); // Устанавливаем адаптивный размер последнего столбца
    table->verticalHeader()->setMinimumSectionSize(1); //Без этого невозможно уменьшить высоту ячейки меньше определенного
    table->horizontalHeader()->setMinimumHeight(1);
    table->horizontalHeader()->setFixedHeight(22);
    table->setMinimumSize(400,rows*rowsH+headerH+2);

    for(uint8_t i=0; i<rows; i++)
    {
        table->setRowHeight(i,rowsH); //Высота ячеек
    }

    QStringList horizHeaders;                               //Объявим список,
    horizHeaders<<"Bit"<<"Name"<<"Status";       //в который положим имена столбцов
    table->setHorizontalHeaderLabels(horizHeaders);         //отобразим имена столбцов

    table->verticalHeader()->hide(); //Имена строк наоборот, спрячем

    table->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: rgb(240, 240, 240);}"); //Цвет фона заголовка
//    table->setEditTriggers(QTableWidget::NoEditTriggers); //Запрет редактирования таблицы

    for (uint8_t i=0; i<rows ; i++)    //Заполним столбцы
    {
        QTableWidgetItem *itm = new QTableWidgetItem(QString("%1").arg(data[i].bit, 1, 10));
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,BIT_NUM,itm);                          //Устанавливаем данные в первый столбец
        table->item(i,BIT_NUM)->setTextAlignment(Qt::AlignCenter);    //Выравниваем по центру

        itm = new QTableWidgetItem(QString(data[i].name));
        itm->setBackground(QColor(255,255,255));                //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,BIT_NAME,itm);                         //Устанавливаем данные в третий столбец

        itm = new QTableWidgetItem("");
        itm->setBackground(QColor(255,255,255) );               //Задний фон белый
        itm->setFlags(Qt::ItemIsEnabled);                       //Снимаем все флаги, выставляя только один флаг
        table->setItem(i,BIT_STATUS,itm);                       //Устанавливаем данные в третий столбец
    }
}

void MainWindow::fillBinTable(QTableWidget *table, const struct bin_data *data, quint8 size)
{
    qint32 dat = 0;
    quint8 bit = 0;
    ReadWord(m_hidSmbus, &dat, data[0].addr);
    for(quint8 i=0; i<size; i++)
    {
        bit = (dat>>data[i].bit) & 1U;
        switch(bit)
        {
        case 0:
            table->item(i,BIT_STATUS)->setText(data[i].state0);
            break;
        case 1:
            table->item(i,BIT_STATUS)->setText(data[i].state1);
            break;
        }
    }
}

void MainWindow::on_readBits_clicked()
{
    if(!transferStart())return;

    quint8 bitTableSize=0;

    bitTableSize = sizeof(BatteryMode)/sizeof(BatteryMode[0]);
    fillBinTable(tableBatteryMode, BatteryMode, bitTableSize);

    bitTableSize = sizeof(BatteryStatus)/sizeof(BatteryStatus[0]);
    fillBinTable(tableBatteryStatus, BatteryStatus, bitTableSize);

    bitTableSize = sizeof(Pack_Status)/sizeof(Pack_Status[0]);
    fillBinTable(tablePackStatus, Pack_Status, bitTableSize);

    bitTableSize = sizeof(Pack_Config)/sizeof(Pack_Config[0]);
    fillBinTable(tablePackConfig, Pack_Config, bitTableSize);

    bitTableSize = sizeof(AFE_Status)/sizeof(AFE_Status[0]);
    fillBinTable(tableAFE_Status, AFE_Status, bitTableSize);

    bitTableSize = sizeof(Ssafe)/sizeof(Ssafe[0]);
    fillBinTable(tableSsafe, Ssafe, bitTableSize);

    transferStop();
}

void MainWindow::on_Unseal_clicked()
{
    QString str; //Объявляем строку для приема текста
    quint32 pass[4]={0};
    QLineEdit *line[4]={ui->UnsealPass1, ui->UnsealPass2, ui->UnsealPass3, ui->UnsealPass4};
    bool ok = 0; //Флаг успешного преобразования из текста в число

    for(quint8 i=0; i<4; i++)
    {
        str = line[i]->text(); //Получаем строку из ячейки
        pass[i] = str.toUInt(&ok, 16); //Преобразуем строку в число
        if (pass[i] >0xffff || ok == false)
        {
            QMessageBox::critical(this,"Error","The value must be between 0x0000 and 0xFFFF");
            line[i]->setText("");
            return;
        }
    }

    if(!transferStart())return;

    const quint8 dataLen = 10;
    quint8 data[dataLen]={0};

    data [0] = 0xff;
    data [1] = 0x08;

    data [2] = pass[0] >> 8;
    data [3] = (BYTE)pass[0] & 0xff;

    data [4] = pass[1] >> 8;
    data [5] = (BYTE)pass[1] & 0xff;

    data [6] = pass[2] >> 8;
    data [7] = (BYTE)pass[2] & 0xff;

    data [8] = pass[3] >> 8;
    data [9] = (BYTE)pass[3] & 0xff;

    writeRequest(m_hidSmbus, data, dataLen);

    transferStop();
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 3) //вкладка калибровки
    {
        connect(calibTimer, SIGNAL(timeout()), this, SLOT(slotCalibTimerAlarm()));
    }
    else
    {
        disconnect(calibTimer, SIGNAL(timeout()), this, SLOT(slotCalibTimerAlarm()));
    }
}

void MainWindow::slotCalibTimerAlarm()
{
    while(HIDstatus == TRANSFER_IN_PROGRESS){};
    HIDstatus       = TRANSFER_IN_PROGRESS; //Ставим флаг, что идет передача данных

    if (CP2112_Init(&m_hidSmbus, cp2112condition)!=STATUS_OK) //Конфигурируем CP2112
    {
        return;
    }

    qint32 tempK=0;
    float tempC=0.0;
    ReadWord(m_hidSmbus, &tempK, 0x08);
    tempC = (float)tempK/10.0-273.1;
    ui->lineMeasuredTemperature->setText(QString::number(tempC,1,1));

    qint32 voltage=0;
    ReadWord(m_hidSmbus, &voltage, 0x09);
    ui->lineMeasuredVoltage->setText(QString::number(voltage));

    qint32 current=0;
    ReadWord(m_hidSmbus, &current, 0x0a);
    ui->lineMeasuredCurrent->setText(QString("%1").arg((qint16)current,1,10));

    transferStop();
}

bool MainWindow::transferStart()
{
    while(HIDstatus == TRANSFER_IN_PROGRESS){};
    HIDstatus       = TRANSFER_IN_PROGRESS; //Ставим флаг, что идет передача данных

    if (CP2112_Init(&m_hidSmbus, cp2112condition)!=STATUS_OK) //Конфигурируем CP2112
    {
        QMessageBox::critical(this,"Error","CP2112 not ready!");
        HIDstatus = TRANSFER_COMPLETED; //Ставим флаг, что передача данных закончена
        return false;
    }
    return true;
}

void MainWindow::transferStop()
{
    HidSmbus_Close(m_hidSmbus);
    HIDstatus = TRANSFER_COMPLETED;
}

void MainWindow::on_writeTemperature_clicked()
{
    writeCalibData(temperature, 0x06, ui->lineActualTemperature);
}

void MainWindow::on_writeVoltage_clicked()
{
    writeCalibData(voltage, 0x04, ui->lineActualVoltage);
}

void MainWindow::on_writeCurrent_clicked()
{
    writeCalibData(current, 0x05, ui->lineActualCurrent);
}

void MainWindow::writeCalibData(quint8 dataType, quint8 addr, QLineEdit *lineEdit)
{
    while(HIDstatus == TRANSFER_IN_PROGRESS){};
    HIDstatus       = TRANSFER_IN_PROGRESS; //Ставим флаг, что идет передача данных

    if (CP2112_Init(&m_hidSmbus, cp2112condition)!=STATUS_OK) //Конфигурируем CP2112
    {
        HIDstatus = TRANSFER_COMPLETED; //Ставим флаг, что передача данных закончена
        return;
    }

    QString str             =   ""; //Объявляем строку для приема текста
    qint32 data             =   0;
    bool ok                 =   0; //Флаг успешного преобразования из текста в число
    str                     =   lineEdit->text();

    if(dataType==temperature)
    {
        float temp = str.toFloat(&ok);
        temp = (temp + 273.1)*10.0;
        data = temp;
    }
    else
    {
        data = str.toInt(&ok, 10);
    }

    if(data>0xffff || !ok)
    {
        QMessageBox::critical(this,"Error","The entered number is not valid");
        HIDstatus = TRANSFER_COMPLETED; //Ставим флаг, что передача данных закончена
        return;
    }

    const quint8 buffLen    =   4;
    quint8 buff[buffLen]    =   {0};

    buff[0] =   0xff;
    buff[1] =   addr;
    buff[2] =   (BYTE)data & 0xff;
    buff[3] =   data >> 8;

    writeRequest(m_hidSmbus, buff, buffLen);
    transferStop();
}

void MainWindow::on_actionSettings_triggered()
{
    configWindowUI.show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        event->accept();
        configWindowUI.close();
        aboutUI.close();
}

void MainWindow::on_actionInfo_triggered()
{
    aboutUI.show();
}

