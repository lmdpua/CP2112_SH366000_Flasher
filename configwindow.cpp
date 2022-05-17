#include "configwindow.h"
#include "ui_configwindow.h"

#include <QFormLayout>
#include <QCheckBox>
#include <QTextEdit>



ConfigWindow::ConfigWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    setWindowTitle("CP2112 Config");
    readINI();
    showUserData();
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::on_Default_clicked()
{
    for(quint8 i=0; i<numcp2112config; i++)
    {
        cp2112config[i].userData = cp2112config[i].defaultData;
    }
    showUserData();
}

void ConfigWindow::showUserData()
{
    ui->lineBitrate->setText(QString ("%1").arg(cp2112config[Bitrate].userData.toInt()));
    ui->lineACK_Address->setText(QString ("%1").arg(cp2112config[ACK_Address].userData.toInt(),2,16,QLatin1Char('0')));
    ui->checkBoxAutoRespond->setChecked(cp2112config[Auto_Respond].userData.toBool());
    ui->lineWriteTimeout->setText(QString ("%1").arg(cp2112config[Write_Timeout].userData.toInt()));
    ui->lineReadTimeout->setText(QString ("%1").arg(cp2112config[Read_Timeout].userData.toInt()));
    ui->lineTransferRetries->setText(QString ("%1").arg(cp2112config[Transfer_Retries].userData.toInt()));
    ui->checkBoxSCL_LowTimeout->setChecked(cp2112config[SCL_Low_Timeout].userData.toBool());
    ui->lineResponseTimeout->setText(QString ("%1").arg(cp2112config[Response_Timeout].userData.toInt()));
    ui->checkBoxRX_TX_Led->setChecked(cp2112config[RX_TX_Leds_On].userData.toBool());
}

void ConfigWindow::on_Exit_clicked()
{
    showUserData();
    this->close();
}


void ConfigWindow::on_Save_clicked()
{
    cp2112config[Bitrate].userData = ui->lineBitrate->text().toInt();
    cp2112config[ACK_Address].userData = ui->lineACK_Address->text().toInt();
    cp2112config[Auto_Respond].userData = ui->checkBoxAutoRespond->isChecked();
    cp2112config[Write_Timeout].userData = ui->lineWriteTimeout->text().toInt();
    cp2112config[Read_Timeout].userData = ui->lineReadTimeout->text().toInt();
    cp2112config[Transfer_Retries].userData = ui->lineTransferRetries->text().toInt();
    cp2112config[SCL_Low_Timeout].userData = ui->checkBoxSCL_LowTimeout->isChecked();
    cp2112config[Response_Timeout].userData = ui->lineResponseTimeout->text().toInt();
    cp2112config[RX_TX_Leds_On].userData = ui->checkBoxRX_TX_Led->isChecked();

    writeINI();
    this->close();
}

