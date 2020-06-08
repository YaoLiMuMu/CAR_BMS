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
} StateMachine;                    // 状态机结构体
extern StateMachine stateMachine;
extern StateTransform stateTran[]; // 柔性数组必须使用extern

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    static QByteArray Test_Array;                // fresh Demand Current/Voltage
    static VCI_CAN_OBJ _BCL[1];                 // BCL Frame Data
    static VCI_CAN_OBJ _BCLP[1];                // BCLP Frame Data
    static VCI_CAN_OBJ _BCS[2];                 // BCS Frame Data
    static VCI_CAN_OBJ _BCP[2];                 // BCP Frame Data
    static VCI_CAN_OBJ _BSM[1];                 // BSM Frame Data
    static VCI_CAN_OBJ _BHM[1];                 // BHM Frame Data
    static VCI_CAN_OBJ _BRM[7];                 // BRM Frame Data
    static VCI_CAN_OBJ _BDC[4];                 // BDC Frame Data
    static VCI_CAN_OBJ _BCPP[1];                // BCPP Frame Data
    static VCI_CAN_OBJ _BCSP[1];                // BCSP Frame Data
    static int Ready_time_ms;
    static bool transFree;                      // CAN transmit Bus free

signals:
    void EXE_Action(Action);                    // send transmit signal

private slots:
    void CloseDev(unsigned Error);              // function for Close USB_CAN bus
    void runStateMachine(EventID evt);          // StateMachine process
    void on_pushButton1_1_clicked();            // function for loading Demand Voltage/Current setting
    QByteArray processVoltage(QString, int);    // transfer Voltages String to BMS Demand Voltage
    QByteArray processCurrent(QString, int);    // transfer Current String to BMS Demand Current
    QByteArray processTemprature(QString);      // trnasfer Temprature String to BMS Tcell
    void UpdateCCS_CV(QByteArray);                 // update CCS voltage and current
    void UpdateCCD_01(QByteArray);
    void UpdateCCD_00(QByteArray);
    void BMS_Ready();
    void Changer_Vision(QByteArray);            // show Agreement Vision V1.10/V1.12
    void on_lineEdit1_6_textChanged(const QString &arg1);  // VIN Code Edit
    void Parser(EventID, QByteArray);           // analyze can data
    void on_pushButton1_2_clicked();

    void on_checkBox1_2_stateChanged();

    void on_checkBox1_1_stateChanged();

    void on_pushButton1_3_clicked();

private:
    Ui::Widget *ui;
    QThread * Tthread;
    QThread * Rthread;
    proceesframe * pframe;
    sentframe * sframe;
    StateMachine * pSM;
    int Vin_code_num;                           // Vin字节数目
    QByteArray Vin_Code_Array;                  // Vin码储存数组
    typedef void (Widget:: *func)(QByteArray);  // CAN解析函数指针, Qmap插入成员函数指针
    QMap<EventID,func> ParseTable;              // Map 函数指针映射表

};

#endif // WIDGET_H
