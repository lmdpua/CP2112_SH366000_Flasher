#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "cp2112.h"
#include <QTimer>
#include <QTime>
#include <configwindow.h>
#include <about.h>

enum Array
{
    EMPTY,
    READY
};

enum Commands
{
    DEVICE_TYPE     = 0x0001,
    FIRMVARE_REV    = 0x0002,
    EDV_LEVEL       = 0x0003,
    SHUTDOWN        = 0x0005,
    SEAL            = 0x062b,
    RESET           = 0x0041
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setTable (QTableWidget *table);
    void setStatusbarTable(QTableWidget *table);
    void fillStatusbarTable(QTableWidget *table, QString cond[]);
    bool readBatteryFlash(HID_SMBUS_DEVICE &device, QByteArray &flashData);
    bool saveToFile(QString *pathToFile, QByteArray *dataToSave);
    bool readFromFile(QString *pathToFile, QByteArray *dataFromFile);
    void readDataFromArray(void);
    void clearItem(int row, int column);
    void setSBSTable (QTableWidget *table);
    void setBinTable(QTableWidget *table, const struct bin_data *data, quint8 size);
    void fillBinTable(QTableWidget *table, const struct bin_data *data, quint8 size);
    bool transferStart();
    void transferStop();
    void writeCalibData(quint8 dataType, quint8 addr, QLineEdit *lineEdit);

private slots:
    void slotTimerAlarm();
    void slotCalibTimerAlarm();

    void on_readFlash_clicked();
    void on_writeFlash_clicked();

    void on_actionSave_triggered();
    void on_actionOpen_triggered();

    void replaceData(int row, int column);

    void send_command (Commands command);

    void on_DeviceType_clicked();

    void on_FirmwareRev_clicked();

    void on_EDVLevel_clicked();

    void on_Shutdown_clicked();

    void on_Seal_clicked();

    void on_Reset_clicked();


    void on_readSMB_clicked();

    void on_readBits_clicked();

    void on_Unseal_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_writeCurrent_clicked();

    void on_writeTemperature_clicked();

    void on_writeVoltage_clicked();

    void on_actionSettings_triggered();

    void closeEvent(QCloseEvent *event);

    void on_actionInfo_triggered();

private:
    Ui::MainWindow *ui;
    ConfigWindow configWindowUI;
    About aboutUI;
    QTimer *timer;
    QTimer *calibTimer;
    QTableWidget *tableBatteryMode;
    QTableWidget *tableBatteryStatus;
    QTableWidget *statusbarTable;
    QTableWidget *tablePackStatus;
    QTableWidget *tablePackConfig;
    QTableWidget *tableAFE_Status;
    QTableWidget *tableSsafe;
};
#endif // MAINWINDOW_H
