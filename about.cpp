#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    setWindowTitle("About");
}

About::~About()
{
    delete ui;
}

void About::on_OK_clicked()
{
    this->close();
}

