#ifndef WIDGET_H
#define WIDGET_H
#include <controlcan.h>
#include <proceesframe.h>
#include <sentframe.h>
#include <QWidget>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
//***
#ifdef WIN32 // for windows
#   include <windows.h>
#   include <process.h>
#   include <stdio.h>
#   include <time.h>
#   include "controlcan.h"
#   pragma comment(lib, "controlcan.lib")
#   define msleep(ms)  Sleep(ms)
typedef HANDLE pthread_t;
#else // for linux
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <strings.h>
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#endif
//***

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    static QString Gmesg;
    static QByteArray Demand_CV;
    static bool CRM_00;
    static bool CRM_AA;
    static bool CML;
    static short BCL2BCS;

private slots:
    void CloseDev(unsigned Error);

    void on_pushButton1_1_clicked();
    QByteArray processVoltage(QString, int);
    QByteArray processCurrent(QString, int);

private:
    Ui::Widget *ui;
    QThread * Tthread;
    QThread * Rthread;
};

#endif // WIDGET_H
