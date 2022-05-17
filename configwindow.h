#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>

#include "cp2112.h"

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();
    void showUserData();

private slots:

    void on_Default_clicked();

    void on_Exit_clicked();

    void on_Save_clicked();

private:
    Ui::ConfigWindow *ui;

};

#endif // CONFIGWINDOW_H


