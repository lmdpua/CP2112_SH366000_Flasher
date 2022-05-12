#include "readdialog.h"
#include "ui_readdialog.h"
#include <QPushButton>

ReadDialog::ReadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadDialog)
{
    ui->setupUi(this);

    //соединяем кнопку ОК и обработчик
    connect(ui->buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),SLOT(buttonBox_accepted()));
    //соединяем кнопку Отмена и обработчик
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(cliked()),SLOT(close()));
}

ReadDialog::~ReadDialog()
{
    delete ui;
}

void ReadDialog::buttonBox_accepted()
{
    emit FilePath(ui->lineEdit->text());
    close();
}

