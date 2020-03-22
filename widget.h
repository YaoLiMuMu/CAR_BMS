#ifndef WIDGET_H
#define WIDGET_H
#include <controlcan.h>
#include <proceesframe.h>
#include <sentframe.h>
#include <QWidget>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QTimer>
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
typedef struct{
    State state;
    int transNum;
    StateTransform * transform;
} StateMachine;

extern StateMachine * pSM;
extern StateMachine stateMachine;
extern StateTransform stateTran[];

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    static QString Gmesg;                       // goal value for multithreads
    static QByteArray Demand_CV;                // fresh Demand Current/Voltage
    static bool CRM_00;                         // CRM(0x00) history
    static bool CRM_AA;                         // CRM(0xAA) history
    static bool CML;                            // CML history
    static short BCL2BCS;                       // BCL connect to BCS

signals:
    void EXE_Action(Action);

private slots:
    void CloseDev(unsigned Error);              // function for Close USB_CAN bus
    void runStateMachine(EventID evt);
    void on_pushButton1_1_clicked();            // function for loading Demand Voltage/Current setting
    QByteArray processVoltage(QString, int);    // transfer Voltages String to BMS Demand Voltage
    QByteArray processCurrent(QString, int);    // transfer Current String to BMS Demand Current
    void BCS_BSM_Gen();

private:
    Ui::Widget *ui;
    QThread * Tthread;
    QThread * Rthread;
    QTimer *myTimer;
};

#endif // WIDGET_H
