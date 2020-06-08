#include "widget.h"
#include "ui_widget.h"

StateMachine stateMachine;
StateTransform stateTran[] = {
        // Jump Process
        {V1, CDC, V1, BDC_INIT},
        {V1, BDC_ACK, V1, BDC},
        {V1, CHM, H1, BHM},
        {V1, Null_5, S1, Busleep},
        {V1, Start_Button, V1, Busleep},
        {H1, Null_1, H2, Busleep},
        {H1, CRM_00, H2, BRM_INIT},
        {H1, Start_Button, H1, Busleep},
        {H2, Null_2, T1, BEM_CRM},
        {H2, Null_3, T1, BEM_CRM},
        {H2, BRM_ACK, H2, BRM},
        {H2, CRM_AA, C1, BCP_INIT},
        {C1, BCP_ACK, C1, BCP},
        {C1, CML, R0, BRO_00},
        {R0, Ready_Button, R1, BRO_AA},
        {R1, CRO_AA, R2, N_A},
        {R2, CCD_01, P2, BCLP},
        {R2, CCD_00, P1, BCL},
        {P1, BCS_ACK, P1, BCS},
        {P1, CCD_01, P2, BCLP},
        {P1, Null_6, T1, BEM_CCS},
        {P1, Null_7, T1, BEM_CCD},
        {P1, CST, E1, BST},
        {P1, Kill_Button, E1, BST},
        {P2, BCS_ACK, P2, BCS},
        {P2, CCD_00, P1, BCL},
        {P2, Null_6, T1, BEM_CCS},
        {P2, Null_7, T1, BEM_CCD},
        {P2, CST, E1, BST},
        {P2, Kill_Button, E1, BST},
        {T1, CRO_00, H2, BRM_INIT},
        {T1, Null_4, T2, BEM_CRM},
        {T2, CRO_00, H2, BRM_INIT},
        {T2, Null_4, T3, BEM_CRM},
        {T3, CRO_00, H2, BRM_INIT},
        {T3, Null_4, S1, Busleep},
        {E1, CST, S1, BSD},
        {E1, CSD, S1, Busleep},
        {S1, Start_Button, V1, Busleep},
    };// 该柔性数组不能在被调函数中声明赋值, 要不被调函数结束后会被回收, 则指向该数组的指针变为空指针
VCI_CAN_OBJ Widget::_BCL[1];
VCI_CAN_OBJ Widget::_BCLP[1];
VCI_CAN_OBJ Widget::_BCS[2];
VCI_CAN_OBJ Widget::_BCP[2];
VCI_CAN_OBJ Widget::_BSM[1];
VCI_CAN_OBJ Widget::_BHM[1];
VCI_CAN_OBJ Widget::_BRM[7];
VCI_CAN_OBJ Widget::_BDC[4];
VCI_CAN_OBJ Widget::_BCPP[1];
VCI_CAN_OBJ Widget::_BCSP[1];
int Widget::Ready_time_ms;
bool Widget::transFree;
QByteArray Widget::Test_Array = QByteArray::fromHex("010102");//6810D80E01 420V Voltage and 1A Current

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
    // Initial pSM;
    pSM = new StateMachine;
    stateMachine.state = V1;
    stateMachine.transNum = 39;
    stateMachine.transform = stateTran;
    pSM = & stateMachine;
    Ready_time_ms = 50000; // BMS准备就绪30s
    transFree = false;
    Vin_code_num = 17;
    Vin_Code_Array.resize(Vin_code_num);
    // Fill the event ParseTable
    {
        Widget::ParseTable.insert(CDC, & Widget::Changer_Vision);
        Widget::ParseTable.insert(CHM, & Widget::Changer_Vision);
        Widget::ParseTable.insert(CCS, & Widget::UpdateCCS_CV);
        Widget::ParseTable.insert(CCD_00, & Widget::UpdateCCD_00);
        Widget::ParseTable.insert(CCD_01, & Widget::UpdateCCD_01);
    }
    // Initialization Frame Data
    {
        // _BCL Frame Data
        {
            _BCL->SendType = gTxType;
            _BCL->RemoteFlag = 0;
            _BCL->ExternFlag = 1;
            _BCL->DataLen = 5;
            _BCL->Data[0] = 0x68;
            _BCL->Data[1] = 0x10;
            _BCL->Data[2] = 0xD8;
            _BCL->Data[3] = 0x0E;
            _BCL->Data[4] = 0x02;
            _BCL->ID= 0x181056F4;
        }
        // _BCLP Frame Data
        {
            _BCLP->SendType = gTxType;
            _BCLP->RemoteFlag = 0;
            _BCLP->ExternFlag = 1;
            _BCLP->DataLen = 5;
            _BCLP->Data[0] = 0x68;   //6810D80E01 420V Voltage and 1A Current, 4C1D600901 750V Voltage and 160A Current
            _BCLP->Data[1] = 0x10;
            _BCLP->Data[2] = 0xD8;
            _BCLP->Data[3] = 0x0E;
            _BCLP->Data[4] = 0x03;
            _BCLP->ID= 0x181056F4;
        }
        // _BCS Frame Data
        {
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
        }
        // _BCP Frame Data
        {
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
        }
        // _BSM Frame Data
        {
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
        }
        // _BHM Frame Data
        {
            _BHM->SendType = gTxType;
            _BHM->RemoteFlag = 0;
            _BHM->ExternFlag = 1;
            _BHM->DataLen = 7;
            _BHM->Data[0] = 0x4C;
            _BHM->Data[1] = 0x1D;
            _BHM->ID = 0x182756F4;
        }
        // _BRM Frame Data
        {
            _BRM[0].SendType = gTxType;
            _BRM[0].RemoteFlag = 0;
            _BRM[0].DataLen = 8;
            _BRM[0].Data[0] = 0x01;
            _BRM[0].Data[1] = 0x01;
            _BRM[0].Data[2] = 0x01;
            _BRM[0].Data[3] = 0x00;
            _BRM[0].Data[4] = 0x01;
            _BRM[0].Data[5] = 0x88;
            _BRM[0].Data[6] = 0x13;
            _BRM[0].Data[7] = 0x4C;
            _BRM[0].ID = 0x1CEB56F4;
            _BRM[0].ExternFlag = 1;
            _BRM[1].SendType = gTxType;
            _BRM[1].RemoteFlag = 0;
            _BRM[1].DataLen = 8;
            _BRM[1].Data[0] = 0x02;
            _BRM[1].Data[1] = 0x1D;
            _BRM[1].Data[2] = 0x4E;
            _BRM[1].Data[3] = 0x53;
            _BRM[1].Data[4] = 0x49;
            _BRM[1].Data[5] = 0x56;
            _BRM[1].Data[6] = 0x01;
            _BRM[1].Data[7] = 0x00;
            _BRM[1].ID = 0x1CEB56F4;
            _BRM[1].ExternFlag = 1;
            _BRM[2].SendType = gTxType;
            _BRM[2].RemoteFlag = 0;
            _BRM[2].DataLen = 8;
            _BRM[2].Data[0] = 0x03;
            _BRM[2].Data[1] = 0x00;
            _BRM[2].Data[2] = 0x00;
            _BRM[2].Data[3] = 0x06;
            _BRM[2].Data[4] = 0x08;
            _BRM[2].Data[5] = 0x01;
            _BRM[2].Data[6] = 0x01;
            _BRM[2].Data[7] = 0x00;
            _BRM[2].ID = 0x1CEB56F4;
            _BRM[2].ExternFlag = 1;
            _BRM[3].SendType = gTxType;
            _BRM[3].RemoteFlag = 0;
            _BRM[3].DataLen = 8;
            _BRM[3].Data[0] = 0x04;
            _BRM[3].Data[1] = 0x00;
            _BRM[3].Data[2] = 0x01;
            _BRM[3].Data[3] = 0xFF;
            _BRM[3].Data[4] = 'N';
            _BRM[3].Data[5] = 'Z';
            _BRM[3].Data[6] = '0';
            _BRM[3].Data[7] = '9';
            _BRM[3].ID = 0x1CEB56F4;
            _BRM[3].ExternFlag = 1;
            _BRM[4].SendType = gTxType;
            _BRM[4].RemoteFlag = 0;
            _BRM[4].DataLen = 8;
            _BRM[4].Data[0] = 0x05;
            _BRM[4].Data[1] = '4';
            _BRM[4].Data[2] = '3';
            _BRM[4].Data[3] = 0x06;
            _BRM[4].Data[4] = 0x07;
            _BRM[4].Data[5] = 0x08;
            _BRM[4].Data[6] = 0x09;
            _BRM[4].Data[7] = 0x0A;
            _BRM[4].ID = 0x1CEB56F4;
            _BRM[4].ExternFlag = 1;
            _BRM[5].SendType = gTxType;
            _BRM[5].RemoteFlag = 0;
            _BRM[5].DataLen = 8;
            _BRM[5].Data[0] = 0x06;
            _BRM[5].Data[1] = 0x00;
            _BRM[5].Data[2] = 0x00;
            _BRM[5].Data[3] = 0x00;
            _BRM[5].Data[4] = 0x00;
            _BRM[5].Data[5] = 0x00;
            _BRM[5].Data[6] = 0x00;
            _BRM[5].Data[7] = 0xFF;
            _BRM[5].ID = 0x1CEB56F4;
            _BRM[5].ExternFlag = 1;
            _BRM[6].SendType = gTxType;
            _BRM[6].RemoteFlag = 0;
            _BRM[6].DataLen = 8;
            _BRM[6].Data[0] = 0x07;
            _BRM[6].Data[1] = 0x0A;
            _BRM[6].Data[2] = 0x0B;
            _BRM[6].Data[3] = 0xDF;
            _BRM[6].Data[4] = 0x07;
            _BRM[6].Data[5] = 0xFF;
            _BRM[6].Data[6] = 0xFF;
            _BRM[6].Data[7] = 0xFF;
            _BRM[6].ID = 0x1CEB56F4;
            _BRM[6].ExternFlag = 1;
        }
        // _BDC Frame Data
        {
            _BDC[0].SendType = gTxType;
            _BDC[0].RemoteFlag = 0;
            _BDC[0].DataLen = 8;
            _BDC[0].Data[0] = 0x01;
            _BDC[0].Data[1] = 0x01;
            _BDC[0].Data[2] = 0x02;
            _BDC[0].Data[3] = 0x03;
            _BDC[0].Data[4] = 0x04;
            _BDC[0].Data[5] = 0x05;
            _BDC[0].Data[6] = 0x06;
            _BDC[0].Data[7] = 0x07;
            _BDC[0].ID = 0x1CEB56F4;
            _BDC[0].ExternFlag = 1;
            _BDC[1].SendType = gTxType;
            _BDC[1].RemoteFlag = 0;
            _BDC[1].DataLen = 8;
            _BDC[1].Data[0] = 0x02;
            _BDC[1].Data[1] = 0x08;
            _BDC[1].Data[2] = 0x09;
            _BDC[1].Data[3] = 0x0A;
            _BDC[1].Data[4] = 0x0B;
            _BDC[1].Data[5] = 0x0C;
            _BDC[1].Data[6] = 0x0D;
            _BDC[1].Data[7] = 0x0E;
            _BDC[1].ID = 0x1CEB56F4;
            _BDC[1].ExternFlag = 1;
            _BDC[2].SendType = gTxType;
            _BDC[2].RemoteFlag = 0;
            _BDC[2].DataLen = 8;
            _BDC[2].Data[0] = 0x03;
            _BDC[2].Data[1] = 0x0F;
            _BDC[2].Data[2] = 0x10;
            _BDC[2].Data[3] = 0x11; // VIN Code
            _BDC[2].Data[4] = 0x32; // SOC=50%
            _BDC[2].Data[5] = 0xE8;
            _BDC[2].Data[6] = 0x03; // Battery capacity = 100Ah
            _BDC[2].Data[7] = 0xE8;
            _BDC[2].ID = 0x1CEB56F4;
            _BDC[2].ExternFlag = 1;
            _BDC[3].SendType = gTxType;
            _BDC[3].RemoteFlag = 0;
            _BDC[3].DataLen = 8;
            _BDC[3].Data[0] = 0x04;
            _BDC[3].Data[1] = 0x03; // Battery capacity = 100Kwh
            _BDC[3].Data[2] = 0xE8;
            _BDC[3].Data[3] = 0x03; // V2G Cycles = 100 times
            _BDC[3].Data[4] = 0xE8;
            _BDC[3].Data[5] = 0x03; // Remaining Cycles = 100 times
            _BDC[3].Data[6] = 0xE8;
            _BDC[3].Data[7] = 0x03; // Rechange Mileage = 100 km
            _BDC[3].ID = 0x1CEB56F4;
            _BDC[3].ExternFlag = 1;
        }
        // _BCPP Frame Data
        {
            _BCPP->SendType = gTxType;
            _BCPP->RemoteFlag = 0;
            _BCPP->ExternFlag = 1;
            _BCPP->DataLen = 7;
            _BCPP->Data[0] = 0x68;
            _BCPP->Data[1] = 0x10;  // Maximum output Current = 20 A
            _BCPP->Data[2] = 0x60;
            _BCPP->Data[3] = 0x09;  // Lowest Voltage
            _BCPP->Data[4] = 0xFA;
            _BCPP->Data[5] = 0x00;  // Lowest Cell Voltage = 2.50 V
            _BCPP->Data[6] = 0x05;  // Lowest SOC = 5%
            _BCPP->ID= 0x183056F4;  // BCP PGN = 0x30
        }
        // _BCSP Frame Data
        {
            _BCSP->SendType = gTxType;
            _BCSP->RemoteFlag = 0;
            _BCSP->ExternFlag = 1;
            _BCSP->DataLen = 5;
            _BCSP->Data[0] = 0xFF;
            _BCSP->Data[1] = 0xFF;  // Maximum output Current = 20 A
            _BCSP->Data[2] = 0xE8;
            _BCSP->Data[3] = 0x03;  // Rechange Mileage = 100 km
            _BCSP->Data[4] = 0xFF;
            _BCSP->ID= 0x1C3456F4;  // BCSP PGN = 0x34
        }
    }
    qDebug() << "Mainthread ID: " << QThread::currentThreadId();
    // Open usbCAN Device
    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        return;
    }
        printf("VCI_OpenDevice succeeded\n");
    // Initial configure Usb_Can
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
    connect(Tthread,SIGNAL(started()),sframe,SLOT(Initalize()));
    connect(pframe,SIGNAL(Send2Main(EventID)), this, SLOT(runStateMachine(EventID)));
    connect(this,SIGNAL(EXE_Action(Action)), sframe, SLOT(tx_thread(Action)));
    connect(sframe,SIGNAL(feedbackBRO_00()), this, SLOT(BMS_Ready()));
    Tthread->start();
    Rthread->start();
    msleep(1000);
//    runStateMachine(CML); // 此段注释用于代码调试
    connect(pframe,SIGNAL(Sendcan(EventID, QByteArray)),this,SLOT(Parser(EventID, QByteArray)));  // receive and analytical data
    connect(pframe,SIGNAL(Send2UI(unsigned)),this,SLOT(CloseDev(unsigned)));      // CLOSE device for receive error
    connect(sframe,SIGNAL(Shoot_Error(unsigned)),this,SLOT(CloseDev(unsigned))); // ClOSE device for transmit error
}

Widget::~Widget() // 析构函数
{
    pframe->deleteLater();  //一定要在QThread线程退出之前释放内存
    sframe->deleteLater();
    Tthread->quit();    //调用quit让线程退出消息循环,否则线程一直在exec循环中
    Tthread->wait();    //调用完quit后紧接着要调用wait来回收线程资源
    Rthread->quit();
    Rthread->wait();
    delete ui;
}

void Widget::Parser(EventID Event, QByteArray CAN_Array)    // 帧数据内容解析与映射处理
{
     if(ParseTable.contains(Event))
     {
         (this->*ParseTable[Event])(CAN_Array);
     }
}

void Widget::Changer_Vision(QByteArray CHM_Array)       // Agreement Version
{
    CHM_Array.resize(3);       // 必须限值QByteArray长度,要不Debug处理报错
    ui->label1_6->setText(QString("%1.%2%3").arg(uchar(CHM_Array.at(0))).arg(uchar(CHM_Array.at(1))).arg(uchar(CHM_Array.at(2))));
}

void Widget::UpdateCCS_CV(QByteArray CCS_Array)
{
    CCS_Array.resize(7);
    double cvtemp;
    cvtemp = (uchar(CCS_Array.at(1))*256 + uchar(CCS_Array.at(0)))/10;
    ui->lcdNumber1_2->display(cvtemp);
    cvtemp = (uchar(CCS_Array.at(3))*256 + uchar(CCS_Array.at(2)))/10 - 400;
    ui->lcdNumber1_1->display(cvtemp);
    switch (uchar(CCS_Array.at(6)&0x03)) {
    case 0x00:
        ui->label1_12->setText("暂停");
        break;
    case 0x01:
        ui->label1_12->setText("充电");
        break;
    case 0x02:
        ui->label1_12->setText("放电");
        break;
    default:
        ui->label1_12->setText("F");
    }
}

void Widget::UpdateCCD_01(QByteArray CCD_Array)
{
    CCD_Array.resize(3);
    switch (CCD_Array.at(0)) {
    case 0x00:
        ui->label1_19->setText("充电");
        break;
    case 0x01:
        ui->label1_19->setText("放电");
        break;
    default:
        ui->label1_19->setText("F");
    }
}

void Widget::UpdateCCD_00(QByteArray CCD_Array)
{
    CCD_Array.resize(3);
    switch (CCD_Array.at(0)) {
    case 0x00:
        ui->label1_19->setText("充电");
        break;
    case 0x01:
        ui->label1_19->setText("放电");
        break;
    default:
        ui->label1_19->setText("F");
    }
}


void Widget::BMS_Ready()
{
    QEventLoop loop;
    QTimer::singleShot(Ready_time_ms, &loop, SLOT(quit()));    // BMS准备就绪时间, Qtimer单次触发
    loop.exec();
    runStateMachine(Ready_Button);
}

void Widget::runStateMachine(EventID evt)
{
    int i;
    StateTransform * pTrans = nullptr; // initialize point
    for (i = 0; i < stateMachine.transNum; i++) {
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
    pSM->state = pTrans->nextState; // 需要转化为下个状态变量, 必须需要pTrans这个中间变量
    emit EXE_Action(pTrans->action);
    qDebug() << "Next State is : " << pSM->state << ", and Action code is " << pTrans->action;
}

void Widget::CloseDev(unsigned Error)
{
    if (!Error)
        return;
    printf("TX/RX stopped, <ENTER> to CloseDevice...\n");
    getchar();
    VCI_ERR_INFO vei;
    if (VCI_ReadErrInfo(gDevType, gDevIdx, work_Channel, &vei))
        printf("The Last Time Err_code = %0x\n", vei.ErrCode);
    VCI_CloseDevice(gDevType, gDevIdx);
    printf("VCI_CloseDevice\n");
}

void Widget::on_pushButton1_1_clicked()
{
    if (ui->lineEdit1_1->text().isEmpty() || ui->lineEdit1_2->text().isEmpty())
        {
        QMessageBox::information(this,"Warning","Please Input Demand Voltage and Current");
        return;
    }
    // 设置电池需求电压, 需求电流
    QString output = ui->lineEdit1_1->text();
    _BCL->Data[2] = uchar(processCurrent(output,10).at(1));
    _BCL->Data[3] = uchar(processCurrent(output,10).at(0));
    output = ui->lineEdit1_2->text();
    _BCL->Data[0] = uchar(processVoltage(output,10).at(1));
    _BCL->Data[1] = uchar(processVoltage(output,10).at(0));
    output = ui->lineEdit1_9->text();
    _BCLP->Data[0] = uchar(processVoltage(output,10).at(1));
    _BCLP->Data[1] = uchar(processVoltage(output,10).at(0));
    output = ui->lineEdit1_8->text();
    _BCLP->Data[2] = uchar(processCurrent(output,10).at(1));
    _BCLP->Data[3] = uchar(processCurrent(output,10).at(0));
    qDebug() << "BMS Demand Voltage and Current set to " + ui->lineEdit1_2->text() + " V " + ui->lineEdit1_1->text() + " A";
    // 设置电池单体最高温度
    if (!ui->label1_3->text().isEmpty())
    {
        output = ui->lineEdit1_3->text();
        _BSM->Data[1] = uchar(processTemprature(output).at(0));
    }
    // 设置电池最高电压(用于绝缘检测电压)
    if (!ui->lineEdit1_4->text().isEmpty())
    {
        output = ui->lineEdit1_4->text();
        _BHM->Data[0] = uchar(processVoltage(output,10).at(1));
        _BHM->Data[1] = uchar(processVoltage(output,10).at(0));
        _BCP[0].Data[7] = uchar(processVoltage(output,10).at(1));
        _BCP[1].Data[1] = uchar(processVoltage(output,10).at(0));
    }
    // 设置电池起始电压
    if (!ui->lineEdit1_5->text().isEmpty())
    {
        output = ui->lineEdit1_5->text();
        _BCP[1].Data[5] = uchar(processVoltage(output,10).at(1));
        _BCP[1].Data[6] = uchar(processVoltage(output,10).at(0));
    }
    // set up BMS default ready time
    if (!ui->lineEdit1_7->text().isEmpty())
    {
        Ready_time_ms = ui->lineEdit1_7->text().toInt() * 1000;
    }
    // input Vin code in LineEdit
    if (ui->label1_17->text() == "17/17")
    {
        QString texttemp = ui->lineEdit1_6->text();
        std::string tempstring = texttemp.toStdString();
        std::string::iterator it;
        for (it = tempstring.begin(); it < tempstring.end(); it++) {
            if(*it == ' '){
                tempstring.erase(it);
                it--;
            }
        }
        Vin_Code_Array = QByteArray::fromHex(QString::fromStdString(tempstring).toLocal8Bit());
        // VIN code update
        {
            _BRM[3].Data[4] = uchar(Vin_Code_Array.at(0));
            _BRM[3].Data[5] = uchar(Vin_Code_Array.at(1));
            _BRM[3].Data[6] = uchar(Vin_Code_Array.at(2));
            _BRM[3].Data[7] = uchar(Vin_Code_Array.at(3));
            _BRM[4].Data[1] = uchar(Vin_Code_Array.at(4));
            _BRM[4].Data[2] = uchar(Vin_Code_Array.at(5));
            _BRM[4].Data[3] = uchar(Vin_Code_Array.at(6));
            _BRM[4].Data[4] = uchar(Vin_Code_Array.at(7));
            _BRM[4].Data[5] = uchar(Vin_Code_Array.at(8));
            _BRM[4].Data[6] = uchar(Vin_Code_Array.at(9));
            _BRM[4].Data[7] = uchar(Vin_Code_Array.at(10));
            _BRM[5].Data[1] = uchar(Vin_Code_Array.at(11));
            _BRM[5].Data[2] = uchar(Vin_Code_Array.at(12));
            _BRM[5].Data[3] = uchar(Vin_Code_Array.at(13));
            _BRM[5].Data[4] = uchar(Vin_Code_Array.at(14));
            _BRM[5].Data[5] = uchar(Vin_Code_Array.at(15));
            _BRM[5].Data[6] = uchar(Vin_Code_Array.at(16));
            _BDC[0].Data[1] = uchar(Vin_Code_Array.at(0));
            _BDC[0].Data[2] = uchar(Vin_Code_Array.at(1));
            _BDC[0].Data[3] = uchar(Vin_Code_Array.at(2));
            _BDC[0].Data[4] = uchar(Vin_Code_Array.at(3));
            _BDC[0].Data[5] = uchar(Vin_Code_Array.at(4));
            _BDC[0].Data[6] = uchar(Vin_Code_Array.at(5));
            _BDC[0].Data[7] = uchar(Vin_Code_Array.at(6));
            _BDC[1].Data[1] = uchar(Vin_Code_Array.at(7));
            _BDC[1].Data[2] = uchar(Vin_Code_Array.at(8));
            _BDC[1].Data[3] = uchar(Vin_Code_Array.at(9));
            _BDC[1].Data[4] = uchar(Vin_Code_Array.at(10));
            _BDC[1].Data[5] = uchar(Vin_Code_Array.at(11));
            _BDC[1].Data[6] = uchar(Vin_Code_Array.at(12));
            _BDC[1].Data[7] = uchar(Vin_Code_Array.at(13));
            _BRM[2].Data[1] = uchar(Vin_Code_Array.at(14));
            _BRM[2].Data[2] = uchar(Vin_Code_Array.at(15));
            _BRM[2].Data[3] = uchar(Vin_Code_Array.at(16));
        }
    }
}

QByteArray Widget::processVoltage(QString item, int k) // or can try str.toLatin1()/str.toUtf8()
{
    QByteArray dat;
    int val = item.toInt() * k; // int < 2147483648
    item = QString::asprintf("%04x", val);  // or String str = Interger.toHexString(val) ; or item = item.setNum(val, 16);  需要考虑补"0"
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
    item = QString::asprintf("%04x", val);    // or String str = Interger.toHexString(val) ; use String
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

void Widget::on_lineEdit1_6_textChanged(const QString &arg1)    // Vin code Edit Line
{
    if(ui->lineEdit1_6->text().isEmpty())
        return;
    QString hexbuff="0123456789ABCDEFabcdef";
    bool flag = 0;
    for (int j=0; j<hexbuff.length(); j++) {
        if (arg1[arg1.length()-1] == hexbuff[j])
            flag = 1;
    }
    if(flag)
    {
        flag = 0;
        if(arg1.length()<2){
            ui->lineEdit1_6->setText(arg1.toUpper());
            ui->label1_17->setText(QString::number(0)+QString::fromUtf8("/")+QString::number(Vin_code_num));
            return;
        }
        QString texttemp = arg1.toUpper();
        std::string tempstring = texttemp.toStdString();
        std::string::iterator it;
        for (it = tempstring.begin(); it < tempstring.end(); it++) {
            if(*it == ' '){
                tempstring.erase(it);
                it--;
            }
        }
        ui->label1_17->setText(QString::number(tempstring.length()/2)+QString::fromUtf8("/")+QString::number(Vin_code_num));
        texttemp = QString::fromStdString(tempstring);
        short mod = (texttemp.length()/2);
        short res = (texttemp.length()%2);
        if(res)
        {
            for (short i=1; i <= mod; i++) {
                texttemp.insert((3*i-1), " ");
            }
        }
        else {
            QString Buff_Temp = arg1.toUpper();
            ui->lineEdit1_6->setText(Buff_Temp);
            return;
        }
        ui->lineEdit1_6->setText(texttemp);
    }
    else {
        QString Delete_End_String = arg1.left(arg1.length()-1);
        ui->lineEdit1_6->setText(Delete_End_String);
    }
}


void Widget::on_pushButton1_2_clicked()     // BMS Ready to charge
{
    runStateMachine(Ready_Button);
}

void Widget::on_checkBox1_2_stateChanged()  // Switch with Normal mode and V2G mode
{
    if(ui->checkBox1_2->isChecked())
    {
        if (stateMachine.state == V1 || stateMachine.state == S1)
        {
            stateMachine.state = H1; // 常规模式
            stateMachine.transform[16].action = BCL;
            stateMachine.transform[16].nextState = P1;
            qDebug() << "BMS had been setup to Normal Mode";
        }
    }
    else {
        if (stateMachine.state == H1 || stateMachine.state == S1)
        {
            stateMachine.state = V1; // V2G模式
            stateMachine.transform[16].action = N_A;
            stateMachine.transform[16].nextState = R2;
            qDebug() << "BMS had been setup to V2G Mode";
        }
    }
}

void Widget::on_checkBox1_1_stateChanged()  // Switch with paues charging and continue charging
{
    if(ui->checkBox1_1->isChecked())
    {
        _BSM->Data[6] = 0xC0; // 暂停
        qDebug() << "BMS request a pause charging";
    }
    else {
        _BSM->Data[6] = 0xD0; // 允许充电
        qDebug() << "BSM allow continue charging";
    }
}

void Widget::on_pushButton1_3_clicked()
{
    if(ui->checkBox1_2->isChecked())
    {
        stateMachine.state = H1; // 常规模式
        stateMachine.transform[16].action = BCL;
        stateMachine.transform[16].nextState = P1;
        qDebug() << "BMS had been setup to Normal Mode";
    }
    else {
        stateMachine.state = V1; // V2G模式
        stateMachine.transform[16].action = N_A;
        stateMachine.transform[16].nextState = R2;
        qDebug() << "BMS had been setup to V2G Mode";
    }
    runStateMachine(Start_Button);
}
