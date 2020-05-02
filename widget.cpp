#include "widget.h"
#include "ui_widget.h"

StateMachine stateMachine;
TransFrame transframe[2];
StateTransform stateTran[] = {
        {H1, Null_1, H2, BRM_INIT},
        {H1, CRM_00, H2, BRM_INIT},
        {H1, CHM, H1, BHM},
        {H2, CRM_00, H2, BRM_INIT},
        {H2, Null_2, T1, BEM},
        {H2, Null_3, T1, BEM},
        {T1, K5_K6, W1, Bus_sleep},
        {H2, BRM_ACK, H2, BRM},
        //{H2, BRM_CONF, H2, BRM_INIT},
        {H2, CRM_AA, C1, BCP_INIT},
        {C1, CRM_AA, C1, BCP_INIT},
        {C1, BCP_ACK, C1, BCP},
        //{C1, BCP_CONF, C1, BCP_INIT},
        {C1, CML, R1, BRO_00},
        //{R1, CML, R1, BRO_00},
        {R1, CRO_00, R1, BRO_AA},
        {R1, CRO_AA, P1, BCL},
        {P1, BCS_ST, P2, BCS_INIT},
        {P1, BCS_ACK, P2, BCS},
        {P2, BCS_ACK, P2, BCS},
        {P2, BCS_CONF, P1, BCL},
        {W1, CRM_00, H2, BRM_INIT},
        {H1, B_BHM, H1, BHM},
        {R1, B_BRO_00, R1, BRO_AA},
        {P1, B_BCL, P1, BCL},
        {P2, B_BCS_INIT, P1, BCL},
        {P2, BCS_ST, P2, BCS_INIT},
        {P1, BCL_ST, P1, BCL},
        {P1, BSM_ST, P1, BSM},
        {P2, BCL_ST, P2, BCL},
    {P2, CHM, H1, Free_50ms},
    {P1, CHM, H1, Free_50ms},
    {R1, CHM, H1, Free_50ms},
    };// 该柔性数组不能在被调函数中声明赋值, 要不被调函数结束后会被回收, 则指向该数组的指针变为空指针
QMutex m_mutex;
VCI_CAN_OBJ Widget::_BCL[1];
VCI_CAN_OBJ Widget::_BCS[2];
VCI_CAN_OBJ Widget::_BCP[2];
VCI_CAN_OBJ Widget::_BSM[1];
VCI_CAN_OBJ Widget::_BHM[1];
bool Widget::Free_work;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qRegisterMetaType<Action>("Action");
    qRegisterMetaType<EventID>("EventID");
    proceesframe * pframe = new proceesframe;
    sentframe * sframe = new sentframe;
    QThread * Tthread = new QThread;
    QThread * Rthread = new QThread;
    pframe->moveToThread(Rthread);
    sframe->moveToThread(Tthread);
    myTimer = new QTimer(this);
    myTimer->setInterval(250);
    byTimer = new QTimer(this);
    byTimer->setInterval(50);
    // Initial pSM;
    pSM = new StateMachine;
    stateMachine.state = H1;
    stateMachine.transNum = 31;
    stateMachine.transform = stateTran;
    pSM = & stateMachine;
    Free_work = false;
    QMutexLocker m_lock(&m_mutex);
    // _BCL Frame Data
    {
        _BCL->SendType = gTxType;
        _BCL->RemoteFlag = 0;
        _BCL->ExternFlag = 1;
        _BCL->DataLen = 5;
    //    _BCL->Data[0] = 0x68;
    //    _BCL->Data[1] = 0x10;
    //    _BCL->Data[2] = 0x96;
    //    _BCL->Data[3] = 0x0F;
    //    _BCL->Data[4] = 0x01;
        _BCL->Data[0] = 0x4C;//4C1D600901
        _BCL->Data[1] = 0x1D;
        _BCL->Data[2] = 0x60;
        _BCL->Data[3] = 0x09;
        _BCL->Data[4] = 0x01;
        _BCL->ID= 0x181056F4;
        // _BCS Frame Data
        _BCS[0].SendType = gTxType;
        _BCS[0].RemoteFlag = 0;
        _BCS[0].ExternFlag = 1;
        _BCS[0].DataLen = 8;
        _BCS[0].Data[0] = 0x01;
        _BCS[0].Data[1] = 0x0F;
        _BCS[0].Data[2] = 0x0F;
        _BCS[0].Data[3] = 0x96;
        _BCS[0].Data[4] = 0x0F;
        _BCS[0].Data[5] = 0x98;
        _BCS[0].Data[6] = 0x08;
        _BCS[0].Data[7] = 0x32;
        _BCS[0].ID = 0x1CEB56F4;
        _BCS[1].SendType = gTxType;
        _BCS[1].RemoteFlag = 0;
        _BCS[1].ExternFlag = 1;
        _BCS[1].DataLen = 8;
        _BCS[1].Data[0] = 0x02;
        _BCS[1].Data[1] = 0x2C;
        _BCS[1].Data[2] = 0x01;
        _BCS[1].Data[3] = 0xFF;
        _BCS[1].Data[4] = 0xFF;
        _BCS[1].Data[5] = 0xFF;
        _BCS[1].Data[6] = 0xFF;
        _BCS[1].Data[7] = 0xFF;
        _BCS[1].ID = 0x1CEB56F4;
        // _BCP Frame Data
        _BCP[0].SendType = gTxType;
        _BCP[0].RemoteFlag = 0;
        _BCP[0].ExternFlag = 1;
        _BCP[0].DataLen = 8;
        _BCP[0].Data[0] = 0x01;
        _BCP[0].Data[1] = 0x60;
        _BCP[0].Data[2] = 0x09;
        _BCP[0].Data[3] = 0xDC;
        _BCP[0].Data[4] = 0x05;
        _BCP[0].Data[5] = 0x10;
        _BCP[0].Data[6] = 0x27;
        _BCP[0].Data[7] = 0x68;
        _BCP[0].ID = 0x1CEB56F4;
        _BCP[1].SendType = gTxType;
        _BCP[1].RemoteFlag = 0;
        _BCP[1].ExternFlag = 1;
        _BCP[1].DataLen = 8;
        _BCP[1].Data[0] = 0x02;
        _BCP[1].Data[1] = 0x10;
        _BCP[1].Data[2] = 0xFA;
        _BCP[1].Data[3] = 0xF4;
        _BCP[1].Data[4] = 0x01;
        _BCP[1].Data[5] = 0xA0;
        _BCP[1].Data[6] = 0x0F;
        _BCP[1].Data[7] = 0xFF;
        _BCP[1].ID = 0x1CEB56F4;
        // _BSM Frame Data
        _BSM->SendType = gTxType;
        _BSM->RemoteFlag = 0;
        _BSM->ExternFlag = 1;
        _BSM->DataLen = 7;
        _BSM->Data[0] = 0x19;//01 82 01 6E 02 20 D0
        _BSM->Data[1] = 0x50;//3C;
        _BSM->Data[2] = 0x0F;
        _BSM->Data[3] = 0x3A;
        _BSM->Data[4] = 0x01;
        _BSM->Data[5] = 0x00;
        _BSM->Data[6] = 0xD0;
        _BSM->ID= 0x181356F4;
        // _BHM Frame Data
        _BHM->SendType = gTxType;
        _BHM->RemoteFlag = 0;
        _BHM->ExternFlag = 1;
        _BHM->DataLen = 7;
        _BHM->Data[0] = 0x4C;
        _BHM->Data[1] = 0x1D;
        _BHM->ID = 0x182756F4;
    }
//    transframe[1].func = Parser;
//    transframe[2].func = Parser;
    qDebug() << "Mainthread ID: " << QThread::currentThreadId();

    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        return;
    }
    // Initial configure Usb_Can
    printf("VCI_OpenDevice succeeded\n");
    VCI_INIT_CONFIG config;
    config.AccCode = 0;
    config.AccMask = 0xffffffff;
    config.Filter = 1;  // 单滤波
    config.Mode = 0;    // 正常模式
    config.Timing0 = gBaud & 0xff;
    config.Timing1 = gBaud >> 8;
    for (uint i = 0; i < MAX_CHANNELS; i++)
    {
        if ((gChMask & (1 << i)) == 0) continue;

        if (!VCI_InitCAN(gDevType, gDevIdx, i, &config))
        {
            printf("VCI_InitCAN(%d) failed\n", i);
            return;
        }
        printf("VCI_InitCAN(%d) succeeded\n", i);

        if (!VCI_StartCAN(gDevType, gDevIdx, i))
        {
            printf("VCI_StartCAN(%d) failed\n", i);
            return;
        }
        printf("VCI_StartCAN(%d) succeeded\n", i);
    }
    // Start multi-thread
    connect(Rthread,SIGNAL(started()),pframe,SLOT(rx_thread()));
    connect(pframe,SIGNAL(Send2Main(EventID)), this, SLOT(runStateMachine(EventID)));
    connect(this,SIGNAL(EXE_Action(Action)), sframe, SLOT(tx_thread(Action)));
    connect(sframe,SIGNAL(FeedBack(EventID)), this, SLOT(runStateMachine(EventID)));

    connect(sframe,SIGNAL(BCS_TimeStamp()), myTimer, SLOT(start()));
    connect(myTimer, SIGNAL(timeout()), this, SLOT(BCS_BSM_Gen()));
    connect(byTimer, SIGNAL(timeout()), this, SLOT(BCL_Gen()));
    Tthread->start();
    Rthread->start();
//    runStateMachine(CHM);
    msleep(1000);
    myTimer->start();
    byTimer->start();
    connect(pframe,SIGNAL(Sendcan(EventID, QByteArray)),this,SLOT(Parser(EventID, QByteArray)));
    connect(pframe,SIGNAL(Send2UI(unsigned)),this,SLOT(CloseDev(unsigned))); // CLOSE device for receive error
    connect(sframe,SIGNAL(Shoot_Error(unsigned)),this,SLOT(CloseDev(unsigned))); // ClOSE device for transmit error
}

Widget::~Widget() // 析构函数
{
    delete ui;
}

void Widget::Parser(EventID Event, QByteArray CAN_Array)
{

}

void Widget::Changer_Vision(QByteArray CHM_Array)
{
    ui->label1_6->setText(QString("%1.%2").arg(CHM_Array.at(0)).arg(CHM_Array.at(1)));
}

void Widget::BCS_BSM_Gen()
{
    runStateMachine(BSM_ST);
    msleep(10);
    runStateMachine(BCS_ST);
}

void Widget::BCL_Gen()
{
    runStateMachine(BCL_ST);
}

void Widget::runStateMachine(EventID evt)
{
    int i;
    StateTransform * pTrans = nullptr; // initialize point
    for (i = 0; i < stateMachine.transNum; i++) {
//        qDebug() << "______________________________" << pSM->state << pSM->transform[i].curState << pSM->transform[i].eventId << evt;
        if ((pSM->transform[i].curState == pSM->state) && (pSM->transform[i].eventId == evt)) {
        pTrans = &pSM->transform[i];
        break;
        }
    }
    if (pTrans == nullptr)
        {
            printf( "Current_State= %u Do not process event: %u\r\n", pSM->state, evt);
            return;
        }
    pSM->state = pTrans->nextState; // 需要转化为下个变量, 必须需要pTrans这个中间变量
    emit EXE_Action(pTrans->action);
    qDebug() << "___________________Next State is : " << pSM->state << ", and Action code is " << pTrans->action;
}

void Widget::CloseDev(unsigned Error)
{
    if (!Error)
        return;
    printf("TX/RX stopped, <ENTER> to CloseDevice...\n");
    getchar();
    VCI_ERR_INFO vei;
    if (!VCI_ReadErrInfo(gDevType, gDevIdx, 0, &vei))
        printf("Err_code=%0x\n", vei.ErrCode);
    VCI_CloseDevice(gDevType, gDevIdx);
    printf("VCI_CloseDevice\n");
}

void Widget::on_pushButton1_1_clicked()
{
    if (ui->lineEdit1_1->text().isEmpty() || ui->lineEdit1_2->text().isEmpty())
        {
        QMessageBox::information(this,"Warning","Please Input Voltage and Current");
        return;
    }
    QString output = ui->lineEdit1_1->text();
//    Demand_CV.data()[2] = processCurrent(output,10).at(1);
//    Demand_CV.data()[3] = processCurrent(output,10).at(0);
    _BCL->Data[2] = uchar(processCurrent(output,10).at(1));
    _BCL->Data[3] = uchar(processCurrent(output,10).at(0));
    output = ui->lineEdit1_2->text();
//    Demand_CV.data()[0] = processVoltage(output,10).at(1);
//    Demand_CV.data()[1] = processVoltage(output,10).at(0);
    _BCL->Data[0] = uchar(processVoltage(output,10).at(1));
    _BCL->Data[1] = uchar(processVoltage(output,10).at(0));
    qDebug() << "BMS Demand Voltage and Current Frame set to: 0x" << _BCL->Data[0]
             << _BCL->Data[1] << _BCL->Data[2] << _BCL->Data[3] << _BCL->Data[4];
    output = ui->lineEdit1_3->text();
    _BSM->Data[1] = uchar(processTemprature(output).at(0));
    if (ui->lineEdit1_4->text().isEmpty())
        return;
    output = ui->lineEdit1_4->text();
    _BHM->Data[0] = uchar(processVoltage(output,10).at(1));
    _BHM->Data[1] = uchar(processVoltage(output,10).at(0));
    _BCP[0].Data[7] = uchar(processVoltage(output,10).at(1));
    _BCP[1].Data[1] = uchar(processVoltage(output,10).at(0));
    if (ui->lineEdit1_5->text().isEmpty())
        return;
    output = ui->lineEdit1_5->text();
    _BCP[1].Data[5] = uchar(processVoltage(output,10).at(1));
    _BCP[1].Data[6] = uchar(processVoltage(output,10).at(0));
}

QByteArray Widget::processVoltage(QString item, int k) // or can try str.toLatin1()/str.toUtf8()
{
    QByteArray dat;
    int val = item.toInt() * k; // int < 2147483648
    item = item.setNum(val, 16);    // or String str = Interger.toHexString(val) ; use String
    if (item.length()%2 != 0)
        item = '0' + item;
    dat.resize(item.length()/2);    // can't use sizeof get Qstring size, because will get Qstring object point size
    char ctempor = 0;
    for (int i = 0; i < item.length()/2; i++)
    {
        if (item.toLocal8Bit().data()[2*i] >= '0' && item.toLocal8Bit().data()[2*i] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i] - 'a');
        }
        dat.data()[i] = ctempor;
        if (item.toLocal8Bit().data()[2*i+1] >= '0' && item.toLocal8Bit().data()[2*i+1] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i+1] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i+1] - 'a');
        }
        dat.data()[(i)] = char((dat.data()[i] << 4) | ctempor);
    }
    return dat;
}

QByteArray Widget::processCurrent(QString item, int k) // or can try str.toLatin1()/str.toUtf8()
{
    QByteArray dat;
    int val = 4000 - item.toInt() * k; // int < 2147483648
    item = item.setNum(val, 16);    // or String str = Interger.toHexString(val) ; use String
    if (item.length()%2 != 0)
        item = '0' + item;
    dat.resize(item.length()/2);    // can't use sizeof get Qstring size, because will get Qstring object point size
    char ctempor = 0;
    for (int i = 0; i < item.length()/2; i++)
    {
        if (item.toLocal8Bit().data()[2*i] >= '0' && item.toLocal8Bit().data()[2*i] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i] - 'a');
        }
        dat.data()[i] = ctempor;
        if (item.toLocal8Bit().data()[2*i+1] >= '0' && item.toLocal8Bit().data()[2*i+1] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i+1] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i+1] - 'a');
        }
        dat.data()[i] = char((dat.data()[i] << 4) | ctempor);
    }
    return dat;
}

QByteArray Widget::processTemprature(QString item)
{
    QByteArray dat;
    int val = item.toInt() + 50; // int < 2147483648
    item = item.setNum(val, 16);    // or String str = Interger.toHexString(val) ; use String
    if (item.length()%2 != 0)
        item = '0' + item;
    dat.resize(item.length()/2);    // can't use sizeof get Qstring size, because will get Qstring object point size
    char ctempor = 0;
    for (int i = 0; i < item.length()/2; i++)
    {
        if (item.toLocal8Bit().data()[2*i] >= '0' && item.toLocal8Bit().data()[2*i] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i] - 'a');
        }
        dat.data()[i] = ctempor;
        if (item.toLocal8Bit().data()[2*i+1] >= '0' && item.toLocal8Bit().data()[2*i+1] <= '9')
            ctempor = item.toLocal8Bit().data()[2*i+1] -48;
        else {
            ctempor = 0xa + (item.toLocal8Bit().data()[2*i+1] - 'a');
        }
        dat.data()[(i)] = char((dat.data()[i] << 4) | ctempor);
    }
    return dat;
}
