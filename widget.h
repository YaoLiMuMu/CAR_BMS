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
extern QMutex m_mutex;

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
    static VCI_CAN_OBJ _BST[1];                 // BST Frame Data
    static VCI_CAN_OBJ _BSD[1];                 // BSD Frame Data
    static int Ready_time_ms;
    static bool transFree;                      // CAN transmit Bus free
    static bool V2G_Mode_Flag;                  // V2G mode : flag = true

signals:
    void EXE_Action(Action);                    // send transmit signal

private slots:
    void CloseDev(unsigned Error);              // function for Close USB_CAN bus
    void runStateMachine(EventID evt);          // StateMachine process
    void on_pushButton1_1_clicked();            // function for loading Demand Voltage/Current setting
    QByteArray processVoltage(QString, int);    // transfer Voltages String to BMS Demand Voltage
    QByteArray processCurrent(QString, int);    // transfer Current String to BMS Demand Current
    QByteArray processCurrentBCLP(QString, int);    // transfer Current String to BMS R若echanger Current
    QByteArray processTemprature(QString);      // trnasfer Temprature String to BMS Tcell
    void UpdateCCS_CV(QByteArray);                 // update CCS voltage and current
    void UpdateCCD_01(QByteArray);
    void UpdateCCD_00(QByteArray);
    void UpdateCML_CV(QByteArray);
    void UpdateCMLP_CV(QByteArray);
    void BMS_Ready();
    void BST_Timeout();
    void Changer_Vision(QByteArray);            // show Agreement Vision V1.10/V1.12
    void on_lineEdit1_6_textChanged(const QString &arg1);  // VIN Code Edit
    void Parser(EventID, QByteArray);           // analyze can data
    void Charger_Info_init();                   // 充电机参数信息界面初始化
    void on_pushButton1_2_clicked();

    void on_checkBox1_2_stateChanged();

    void on_pushButton1_3_clicked();

    void on_checkBox1_3_stateChanged(int arg1);

    void on_checkBox1_5_stateChanged(int arg1);

    void on_checkBox1_4_stateChanged(int arg1);

    void on_checkBox1_6_stateChanged(int arg1);

    void on_checkBox1_7_stateChanged(int arg1);

    void on_checkBox1_8_stateChanged(int arg1);

    void on_checkBox1_9_stateChanged(int arg1);

    void on_checkBox1_10_stateChanged(int arg1);

    void on_checkBox1_11_stateChanged(int arg1);

    void on_checkBox1_12_stateChanged(int arg1);

    void on_comboBox1_1_currentIndexChanged(const QString &arg1);

    void on_comboBox1_2_currentIndexChanged(const QString &arg1);

    void on_comboBox1_3_currentIndexChanged(const QString &arg1);

    void on_checkBox1_13_stateChanged(int arg1);

    void on_checkBox1_14_stateChanged(int arg1);

    void on_checkBox1_16_stateChanged(int arg1);

    void on_checkBox1_15_stateChanged(int arg1);

    void on_checkBox1_17_stateChanged(int arg1);

    void on_checkBox1_1_stateChanged(int arg1);

    void on_comboBox1_4_currentIndexChanged(const QString &arg1);

private:
    Ui::Widget *ui;
    QThread * Tthread;
    QThread * Rthread;
    proceesframe * pframe;
    sentframe * sframe;
    StateMachine * pSM;
    int BST_BSD_time_ms;
    int Vin_code_num;                           // Vin字节数目
    QByteArray Vin_Code_Array;                  // Vin码储存数组
    typedef void (Widget:: *func)(QByteArray);  // CAN解析函数指针, Qmap插入成员函数指针
    QMap<EventID,func> ParseTable;              // Map 函数指针映射表
    QMap<QString, uchar> Battery_Type;          // 电池类型
    QMap<QString, uchar> BMS_Version_Set;       // BMS协议版本
    QMap<QString, uchar> BCL_Mode_Flag;         // BCL 充电标识
    QMap<QString, uchar> USBCAN_ChFlag;         // usb_Can 通道标识
};

#endif // WIDGET_H
