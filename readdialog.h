#ifndef READDIALOG_H
#define READDIALOG_H

#include <QDialog>

namespace Ui {
class ReadDialog;
}

class ReadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReadDialog(QWidget *parent = nullptr);
    ~ReadDialog();

private:
    Ui::ReadDialog *ui;
signals:
    void FilePath(const QString &filePath); //Объявим сигнал - получение адреса(строки)

private slots:
    void buttonBox_accepted(); //Объявим слот для нажатой кнопки ОК
};

#endif // READDIALOG_H
