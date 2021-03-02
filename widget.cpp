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
        {H1, CHM, H1, BHM},
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
        {P1, Null_6, T1, BEM}, //BEM_CCS
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
        {H2, CST, E1, BST},
        {C1, CST, E1, BST},
        {R0, CST, E1, BST},
        {E1, Kill_Button, S1, Busleep}
        //{C1, CEM, H1, N_A}
    };// 该柔性数组不能在被调函数中声明赋值, 要不被调函数结束后会被回收, 则指向该数组的指针变为空指针
unsigned gChMask = 0x03;
unsigned work_Channel = 1;
QMutex m_mutex;
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
VCI_CAN_OBJ Widget::_BST[1];
VCI_CAN_OBJ Widget::_BSD[1];
VCI_CAN_OBJ Widget::_BEM[1];
int Widget::Ready_time_ms;
bool Widget::transFree;
bool Widget::V2G_Mode_Flag;
bool Widget::TimeoutBCL_Flag;
QByteArray Widget::Test_Array = QByteArray::fromHex("010102");//6810D80E01 420V Voltage and 1A Current

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("BMS Emulator, Clicking F1 for help"));
    setWindowIcon(QIcon(":/windmill.png"));
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
    stateMachine.transNum = 44;//44
    stateMachine.transform = stateTran;
    pSM = & stateMachine;
    Ready_time_ms = 30000; // BMS准备就绪30s
    transFree = false;
    V2G_Mode_Flag = true;
    TimeoutBCL_Flag = false;
    BST_BSD_time_ms = 5000;
    Vin_code_num = 17;
    Vin_Code_Array.resize(Vin_code_num);
    // 电池类型
    {
        Battery_Type.insert("01: 铅酸电池", 0x01);
        Battery_Type.insert("02: 镍氢电池", 0x02);
        Battery_Type.insert("03: 磷酸铁锂电池", 0x03);
        Battery_Type.insert("04: 锰酸锂电池", 0x04);
        Battery_Type.insert("05: 钴酸锂电池", 0x05);
        Battery_Type.insert("06: 三元材料电池", 0x06);
        Battery_Type.insert("07: 聚合物锂离子电池", 0x07);
        Battery_Type.insert("08: 钛酸锂电池", 0x08);
        Battery_Type.insert("FF: 其他电池", 0xFF);
    }
    // 协议版本
    {
        BMS_Version_Set.insert("1.10 2015国标", 0x01);
        BMS_Version_Set.insert("1.00 2010旧标", 0x02);
        BMS_Version_Set.insert("1.12 2019试行", 0x03);
    }
    // BCL 充电标识
    {
        BCL_Mode_Flag.insert("02 恒流充电", 0x02);
        BCL_Mode_Flag.insert("01 恒压充电", 0x01);
        BCL_Mode_Flag.insert("03 恒流放电", 0x03);
    }
    // USBCAN 通道标识
    {
        USBCAN_ChFlag.insert("CAN1", 0x00);
        USBCAN_ChFlag.insert("CAN2", 0x01);
    }
    // Fill the event ParseTable
    {
        Widget::ParseTable.insert(CDC, & Widget::Changer_Vision);
        Widget::ParseTable.insert(CHM, & Widget::Changer_Vision);
        Widget::ParseTable.insert(CCS, & Widget::UpdateCCS_CV);
        Widget::ParseTable.insert(CCD_00, & Widget::UpdateCCD_00);
        Widget::ParseTable.insert(CCD_01, & Widget::UpdateCCD_01);
        Widget::ParseTable.insert(CML, & Widget::UpdateCML_CV);
        Widget::ParseTable.insert(CMLP, & Widget::UpdateCMLP_CV);
    }
    // Initialization Frame Data
    {
        // _BST Frame Data
        {
            _BST->SendType = gTxType;
            _BST->RemoteFlag = 0;
            _BST->ExternFlag = 1;
            _BST->DataLen = 4;
            _BST->Data[0] = 0x40;
            _BST->Data[1] = 0x00;
            _BST->Data[2] = 0x00;
            _BST->Data[3] = 0xF0;
            _BST->ID = 0x101956F4;
        }
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
            _BCP[0].Data[6] = 0x27;   // 1000Kwh
            _BCP[0].Data[7] = 0x68;
            _BCP[0].ID = 0x1CEB56F4;
            _BCP[1].SendType = gTxType;
            _BCP[1].RemoteFlag = 0;
            _BCP[1].ExternFlag = 1;
            _BCP[1].DataLen = 8;
            _BCP[1].Data[0] = 0x02;
            _BCP[1].Data[1] = 0x10;  // 最高允许电压420V
            _BCP[1].Data[2] = 0xFA;  // 200℃
            _BCP[1].Data[3] = 0xF4;
            _BCP[1].Data[4] = 0x01; // 50% SOC
            _BCP[1].Data[5] = 0xA0;
            _BCP[1].Data[6] = 0x0F; // 当前电压400V
            _BCP[1].Data[7] = 0xFF;
            _BCP[1].ID = 0x1CEB56F4;
        }
        // _BCS Frame Data
        {
            _BCS[0].SendType = gTxType;
            _BCS[0].RemoteFlag = 0;
            _BCS[0].ExternFlag = 1;
            _BCS[0].DataLen = 8;
            _BCS[0].Data[0] = 0x01;
            _BCS[0].Data[1] = _BCP[1].Data[5];
            _BCS[0].Data[2] = _BCP[1].Data[6];      // 电压初始化
            _BCS[0].Data[3] = 0x96;
            _BCS[0].Data[4] = 0x0F;
            _BCS[0].Data[5] = 0x98;
            _BCS[0].Data[6] = 0x08;
            _BCS[0].Data[7] = 0x32;                 // 实时SOC=50%
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
            _BSM->Data[6] = 0xC0;   // 默认暂停充电
            _BSM->ID= 0x181356F4;
        }
        // _BHM Frame Data
        {
            _BHM->SendType = gTxType;
            _BHM->RemoteFlag = 0;
            _BHM->ExternFlag = 1;
            _BHM->DataLen = 2   ;
            _BHM->Data[0] = 0x68;
            _BHM->Data[1] = 0x10;       // 最高允许电压420V
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
            _BRM[0].Data[6] = 0x13;     // 系统额定容量 500Ah
            _BRM[0].Data[7] = 0x4C;
            _BRM[0].ID = 0x1CEB56F4;
            _BRM[0].ExternFlag = 1;
            _BRM[1].SendType = gTxType;
            _BRM[1].RemoteFlag = 0;
            _BRM[1].DataLen = 8;
            _BRM[1].Data[0] = 0x02;
            _BRM[1].Data[1] = 0x1D;     // 系统额定总电压 750V
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
            _BRM[3].Data[4] = 0x59; // Y
            _BRM[3].Data[5] = 0x61; // a
            _BRM[3].Data[6] = 0x6F; // o
            _BRM[3].Data[7] = 0x4C; // L
            _BRM[3].ID = 0x1CEB56F4;
            _BRM[3].ExternFlag = 1;
            _BRM[4].SendType = gTxType;
            _BRM[4].RemoteFlag = 0;
            _BRM[4].DataLen = 8;
            _BRM[4].Data[0] = 0x05;
            _BRM[4].Data[1] = 0x69; // i
            _BRM[4].Data[2] = 0x43; // Cif (Widget::V2G_Mode_Flag)
            _BRM[4].Data[3] = 0x6F; // o
            _BRM[4].Data[4] = 0x64; // d
            _BRM[4].Data[5] = 0x69; // i
            _BRM[4].Data[6] = 0x6E; // n
            _BRM[4].Data[7] = 0x67; // g
            _BRM[4].ID = 0x1CEB56F4;
            _BRM[4].ExternFlag = 1;
            _BRM[5].SendType = gTxType;
            _BRM[5].RemoteFlag = 0;
            _BRM[5].DataLen = 8;
            _BRM[5].Data[0] = 0x06;
            _BRM[5].Data[1] = 0x4E; // N
            _BRM[5].Data[2] = 0x65; // e
            _BRM[5].Data[3] = 0x62; // b
            _BRM[5].Data[4] = 0x75; // u
            _BRM[5].Data[5] = 0x6c; // l
            _BRM[5].Data[6] = 0x61; // a Vin code
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
            _BDC[0].Data[1] = 0x59; // Y
            _BDC[0].Data[2] = 0x61; // a
            _BDC[0].Data[3] = 0x6f; // o
            _BDC[0].Data[4] = 0x4c; // L
            _BDC[0].Data[5] = 0x69; // i
            _BDC[0].Data[6] = 0x43; // C
            _BDC[0].Data[7] = 0x6f; // o
            _BDC[0].ID = 0x1CEB56F4;
            _BDC[0].ExternFlag = 1;
            _BDC[1].SendType = gTxType;
            _BDC[1].RemoteFlag = 0;
            _BDC[1].DataLen = 8;
            _BDC[1].Data[0] = 0x02;
            _BDC[1].Data[1] = 0x64; // dif (Widget::V2G_Mode_Flag)
            _BDC[1].Data[2] = 0x69; // i
            _BDC[1].Data[3] = 0x6E; // n
            _BDC[1].Data[4] = 0x67; // g
            _BDC[1].Data[5] = 0x4E; // N
            _BDC[1].Data[6] = 0x65; // e
            _BDC[1].Data[7] = 0x62; // b
            _BDC[1].ID = 0x1CEB56F4;
            _BDC[1].ExternFlag = 1;
            _BDC[2].SendType = gTxType;
            _BDC[2].RemoteFlag = 0;
            _BDC[2].DataLen = 8;
            _BDC[2].Data[0] = 0x03;
            _BDC[2].Data[1] = 0x75; // u
            _BDC[2].Data[2] = 0x6c; // l
            _BDC[2].Data[3] = 0x61; // a VIN Code
            _BDC[2].Data[4] = 0x32; // SOC=50%
            _BDC[2].Data[5] = 0xE8;
            _BDC[2].Data[6] = 0x03; // Battery capacity = 100Ah
            _BDC[2].Data[7] = 0xE8;
            _BDC[2].ID = 0x1CEB56F4;
            _BDC[2].ExternFlag = 1;
            _BDC[3].SendType = gTxType;
            _BDC[3].RemoteFlag = 0;
            _BDC[3].DataLen = 8;if (Widget::V2G_Mode_Flag)
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
            _BCPP->Data[3] = 0x09;  // Lowest Voltage 240V
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
            _BCSP->DataLen = 8;
            _BCSP->Data[0] = 0x2C;
            _BCSP->Data[1] = 0x01;  // remain rechange time 300min图
            _BCSP->Data[2] = 0xE8;
            _BCSP->Data[3] = 0x03;  // Rechange Mileage = 100 km
            _BCSP->Data[4] = 0xFF;
            _BCSP->Data[5] = 0xFF;
            _BCSP->Data[6] = 0xFF;
            _BCSP->Data[7] = 0xFF;
            _BCSP->ID= 0x1C3456F4;  // BCSP PGN = 0x34
        }
        // _BSD Frame Data
        {
            _BSD->SendType = gTxType;
            _BSD->RemoteFlag = 0;
            _BSD->ExternFlag = 1;
            _BSD->DataLen = 7;
            _BSD->Data[0] = _BCS[0].Data[7];    // 中止SOC
            _BSD->Data[1] = 0x60;
            _BSD->Data[2] = 0x09;   // 最高单体电压24V
            _BSD->Data[3] = 0xC8;
            _BSD->Data[4] = 0x00;   // 最低单体电压2V
            _BSD->Data[5] = 0x64;   // 最高单体温度50℃
            _BSD->Data[6] = 0x32;   // 最低单体温度0℃
            _BSD->ID = 0x181C56F4;
        }
        // _BEM Frame Data initialization
        {
            _BEM->SendType = gTxType;
            _BEM->RemoteFlag = 0;
            _BEM->ExternFlag = 1;
            _BEM->DataLen = 4;
            _BEM->Data[0] = 0xF0;
            _BEM->Data[1] = 0xF0;
            _BEM->Data[2] = 0xF0;
            _BEM->Data[3] = 0x00;
            _BEM->ID = 0x081E56F4;
        }
    }
    qDebug() << "Mainthread ID: " << QThread::currentThreadId();
    // Open usbCAN Device
    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        ui->label1_48->setText("Disconnect");
        return;
    }
        printf("VCI_OpenDevice succeeded\n");
        ui->label1_48->setText("Standby");
    // Initial configure Usb_Can
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
    connect(sframe,SIGNAL(feedbackBST_BSD()), this, SLOT(BMS_Ready()));
    Tthread->start();
    Rthread->start();
    msleep(1000);
    //runStateMachine(BDC_ACK); // 此段用于代码调试或注释
    connect(pframe,SIGNAL(Sendcan(EventID, QByteArray)),this,SLOT(Parser(EventID, QByteArray)));  // receive and analytical data
    connect(sframe,SIGNAL(finished()), Tthread, SLOT(quit()));
    connect(Tthread,SIGNAL(finished()),sframe,SLOT(deleteLater()));
    connect(Tthread,SIGNAL(finished()),Tthread,SLOT(deleteLater()));
    connect(pframe,SIGNAL(Send2UI(unsigned)),this,SLOT(CloseDev(unsigned)));      // CLOSE device for receive error
    connect(sframe,SIGNAL(Shoot_Error(unsigned)),this,SLOT(CloseDev(unsigned))); // ClOSE device for transmit error
    // keybinding singal&slot function
    helpform * helpwindow = new helpform;
    QShortcut * HotkeyF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    connect(HotkeyF1, SIGNAL(activated()), helpwindow, SLOT(exec()));
    fileName = QCoreApplication::applicationDirPath();
    fileName+="/SaveInfo/bms_list.conf";
    QShortcut * HotkeyF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(HotkeyF11, SIGNAL(activated()), this, SLOT(SaveSetting()));
    QShortcut * HotkeyF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    connect(HotkeyF12, SIGNAL(activated()), this, SLOT(ImportConf()));
    QShortcut * HotkeyF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(HotkeyF5, SIGNAL(activated()), this, SLOT(RebootCAN()));
    Fault_State_AUncheck();
}

Widget::~Widget() // 析构函数
{
//    pframe->deleteLater();  //一定要在QThread线程退出之前释放内存
//    sframe->deleteLater();
//    Tthread->quit();    //调用qui最高(低)单体动力蓄电池电压及其组号t让线程退出消息循环,否则线程一直在exec循环中
//    Tthread->wait();    //调用完quit后紧接着要调用wait来回收线程资源
//    Rthread->quit();
//    Rthread->wait();
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
    CCS_Array.resize(8);
    _BCS[0].Data[1] = uchar(CCS_Array.at(0));
    _BCS[0].Data[2] = uchar(CCS_Array.at(1));
    _BCS[0].Data[3] = uchar(CCS_Array.at(2));
    _BCS[0].Data[4] = uchar(CCS_Array.at(3));       // BCS Voltage and current change with CCS
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

void Widget::SaveSetting()
{
    // genrate application configure file
    QSettings settings(fileName, QSettings::NativeFormat);
    settings.setValue("BCL/charging voltage", ui->lineEdit1_2->text());
    settings.setValue("BCL/charging current", ui->lineEdit1_1->text());
    settings.setValue("BHM/max voltage", ui->lineEdit1_4->text());
    settings.setValue("BCP/start voltage", ui->lineEdit1_5->text());
    settings.setValue("BCS/SOC", ui->lineEdit1_17->text());
    settings.setValue("workmode", ui->checkBox1_2->isChecked());
    settings.setValue("USBCAN/channel", ui->comboBox1_4->currentIndex());
    settings.setValue("BRM/vin code", ui->lineEdit1_6->text());
    settings.setValue("BRO/ready time", ui->lineEdit1_7->text());
    settings.setValue("faultact", ui->checkBox1_18->isChecked());
    settings.setValue("BCP/max current", ui->lineEdit1_14->text());
    QMessageBox::information(this,"Warning","Save setting successfully");
}

void Widget::ImportConf()
{
    QSettings settings(fileName, QSettings::NativeFormat);
    ui->lineEdit1_2->setText(settings.value("BCL/charging voltage").toString());
    ui->lineEdit1_1->setText(settings.value("BCL/charging current").toString());
    ui->lineEdit1_4->setText(settings.value("BHM/max voltage").toString());
    ui->lineEdit1_5->setText(settings.value("BCP/start voltage").toString());
    ui->lineEdit1_17->setText(settings.value("BCS/SOC").toString());
    if (settings.value("workmode").toBool())
    {
        ui->checkBox1_2->setCheckState(Qt::Checked);
    }
    else {
        ui->checkBox1_2->setCheckState(Qt::Unchecked);
    }
    ui->comboBox1_4->setCurrentIndex(settings.value("USBCAN/channel").toInt());
    ui->lineEdit1_6->setText(settings.value("BRM/vin code").toString());
    ui->lineEdit1_7->setText(settings.value("BRO/ready time").toString());
    if (settings.value("faultact").toBool())
    {
        ui->checkBox1_18->setCheckState(Qt::Checked);
    }
    else {
        ui->checkBox1_18->setCheckState(Qt::Unchecked);
    }
    ui->lineEdit1_14->setText(settings.value("BCP/max current").toString());
    QMessageBox::information(this,"Warning","Import configuration successfully");
    on_pushButton1_1_clicked();             // Load configuration
}

void Widget::BCS_CV_Init(bool mode)
{
    if (!mode)
    {
        _BCS[0].Data[1] = _BCP[1].Data[5];
        _BCS[0].Data[2] = _BCP[1].Data[6];      //BCS Voltage initialize
        _BCS[0].Data[3] = 0xA0;
        _BCS[0].Data[4] = 0x0F;                 //BCS current: 0A
    }
}

void Widget::RebootCAN()
{
    VCI_CloseDevice(gDevType, gDevIdx);
    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        ui->label1_48->setText("Disconnect");
        return;
    }
        printf("VCI_OpenDevice succeeded\n");
        ui->label1_48->setText("Standby");
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
}

void Widget::UpdateCCD_01(QByteArray CCD_Array)
{
    QMutexLocker m_lock(&m_mutex);
    CCD_Array.resize(3);
    if (!ui->checkBox1_18->isChecked())
    {
        switch (CCD_Array.at(0)) {
        case 0x00:
            ui->label1_19->setText("充电");
            _BSM->Data[6] = 0xD0;
            break;
        case 0x01:
            ui->label1_19->setText("放电");
           _BSM->Data[6] = 0xE0;
            break;
        default:
            ui->label1_19->setText("F");
        }
    }
}

void Widget::UpdateCCD_00(QByteArray CCD_Array)
{
    QMutexLocker m_lock(&m_mutex); // _BSM 全局变量互锁
    CCD_Array.resize(3);
    if (!ui->checkBox1_18->isChecked())
    {
        switch (CCD_Array.at(0)) {
        case 0x00:
            ui->label1_19->setText("充电");
            _BSM->Data[6] = 0xD0;
            break;
        case 0x01:
            ui->label1_19->setText("放电");
            _BSM->Data[6] = 0xE0;
            break;
        default:
            ui->label1_19->setText("F");
        }
    }
}

void Widget::UpdateCML_CV(QByteArray CML_Array)
{
    CML_Array.resize(8);
    double cvtemp;
    cvtemp = (uchar(CML_Array.at(1))*256 + uchar(CML_Array.at(0)))/10;
    ui->label1_21->setText(QString("%1V").arg(cvtemp));
    cvtemp = (uchar(CML_Array.at(3))*256 + uchar(CML_Array.at(2)))/10;
    ui->label1_23->setText(QString("%1V").arg(cvtemp));
    cvtemp = 400 - (uchar(CML_Array.at(5))*256 + uchar(CML_Array.at(4)))/10;
    ui->label1_24->setText(QString("%1A").arg(cvtemp));
    cvtemp = 400 - (uchar(CML_Array.at(7))*256 + uchar(CML_Array.at(6)))/10;
    ui->label1_25->setText(QString("%1A").arg(cvtemp));
}

void Widget::UpdateCMLP_CV(QByteArray CMLP_Array)
{
    CMLP_Array.resize(8);
    double cvtemp;
    cvtemp = (uchar(CMLP_Array.at(1))*256 + uchar(CMLP_Array.at(0)))/10;
    ui->label1_26->setText(QString("%1V").arg(cvtemp));
    cvtemp = (uchar(CMLP_Array.at(3))*256 + uchar(CMLP_Array.at(2)))/10;
    ui->label1_27->setText(QString("%1V").arg(cvtemp));
    cvtemp = (uchar(CMLP_Array.at(5))*256 + uchar(CMLP_Array.at(4)))/10 - 400;
    ui->label1_28->setText(QString("%1A").arg(cvtemp));
    cvtemp = (uchar(CMLP_Array.at(7))*256 + uchar(CMLP_Array.at(6)))/10 - 400;
    ui->label1_29->setText(QString("%1A").arg(cvtemp));
}

void Widget::BST_Timeout()
{
    QEventLoop loop;
    QTimer::singleShot(BST_BSD_time_ms, &loop, SLOT(quit()));    // BST循环发送时长
    loop.exec();
    runStateMachine(Kill_Button);
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
    if (evt == CEM)
    {
        pSM->state = H1;    // 收到CEM报文进入重连
        BCS_CV_Init(false);
        qDebug() << "BSM Is Reconnecting Now...";
    }
    if (pTrans == nullptr)
        {
            printf( "Current_State= %u Do not process event: %u\r\n", pSM->state, evt);
            return;
        }
    pSM->state = pTrans->nextState; // 需要转化为下个状态变量, 必须需要pTrans这个中间变量
    emit EXE_Action(pTrans->action);
    if (pSM->state == R0)
    {
        ui->pushButton1_2->setEnabled(true);
    }
    else {
        ui->pushButton1_2->setEnabled(false);
    }
    if ((pSM->state != S1 && pSM->state != V1)&&pSM->state!=H1)
    {
        ui->label1_48->setText("Working");
    }
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
    // runStateMachine(CCD_00); // 此段用于代码调试或注释
    if (ui->lineEdit1_1->text().isEmpty() || ui->lineEdit1_2->text().isEmpty())
        {
        QMessageBox::information(this,"Warning","Please Input Demand Voltage and Current\nEspecially the Red part");
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
    _BCLP->Data[2] = uchar(processCurrentBCLP(output,10).at(1));
    _BCLP->Data[3] = uchar(processCurrentBCLP(output,10).at(0));
    qDebug() << "BMS Demand Voltage and Current set to " + ui->lineEdit1_2->text() + " V " + ui->lineEdit1_1->text() + " A";
    // BSM 设置电池单体最高温度
    if (!ui->lineEdit1_3->text().isEmpty())
    {
        output = ui->lineEdit1_3->text();
        _BSM->Data[1] = uchar(processTemprature(output).at(0));
        qDebug() << "BSM highest Temprature is " << _BSM->Data[1];
    }
    if (!ui->lineEdit1_28->text().isEmpty())
    {
        _BSM->Data[2] = uchar(ui->lineEdit1_28->text().toInt());
    }
    if (!ui->lineEdit1_31->text().isEmpty())
    {
        output = ui->lineEdit1_31->text();
        _BSM->Data[3] = uchar(processTemprature(output).at(0));
        qDebug() << "BSM lowest Temprature is " << _BSM->Data[3];
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
    // BCS parameter
    if (!ui->lineEdit1_10->text().isEmpty())
    {
        _BCS[0].Data[5] = ui->lineEdit1_10->text().toInt()&0xFF;
        _BCS[0].Data[6] = (ui->lineEdit1_10->text().toInt()/256)&0xFF;      // 单体最高/最低电压
    }
    if (!ui->lineEdit1_12->text().isEmpty())
    {
        _BCS[1].Data[2] = uchar(ui->lineEdit1_12->text().toInt()/256);
        _BCS[1].Data[1] = uchar(ui->lineEdit1_12->text().toInt()%256);      // 剩余时间min
    }
    if (!ui->lineEdit1_17->text().isEmpty())
    {
        _BCS[0].Data[7] = uchar(ui->lineEdit1_17->text().toInt());          // SOC
    }
    if (!ui->lineEdit1_22->text().isEmpty())
    {
        _BCS[0].Data[6] = (ui->lineEdit1_22->text().toInt()*16&0xFF)^_BCS[0].Data[6];
        qDebug() << _BCS[0].Data[6] << _BCS[0].Data[5];
    }
    // BCP parameter
    if (!ui->lineEdit1_15->text().isEmpty())
    {
        _BCP[0].Data[1] = ui->lineEdit1_15->text().toInt()&0xFF;
        _BCP[0].Data[2] = (ui->lineEdit1_15->text().toInt()/256)&0xFF;
    }
    if (!ui->lineEdit1_14->text().isEmpty())
    {
        output = ui->lineEdit1_14->text();
        _BCP[0].Data[3] = uchar(processCurrent(output,10).at(1));
        _BCP[0].Data[4] = uchar(processCurrent(output,10).at(0));   // 最大允许充电电流
    }
    if (!ui->lineEdit1_16->text().isEmpty())
    {
        output = ui->lineEdit1_16->text();
        _BCP[1].Data[2] = uchar(processTemprature(output).at(0));   // 单体温度上限
        qDebug() << "BCP highest Temprature is " << _BCP[1].Data[2];
    }
    if (!ui->lineEdit1_25->text().isEmpty())
    {
        _BCP[0].Data[5] = (ui->lineEdit1_25->text().toInt()*10)&0xFF;
        _BCP[0].Data[6] = (ui->lineEdit1_25->text().toInt()*10/256)&0xFF;
    }
    // BDC parameter
    if (!ui->lineEdit1_11->text().isEmpty())
    {
        _BDC[3].Data[6] = ui->lineEdit1_11->text().toInt()*10&0xFF;
        _BDC[3].Data[7] = (ui->lineEdit1_11->text().toInt()*10/256)&0xFF;      // 续航里程
    }
    if (!ui->lineEdit1_26->text().isEmpty())
    {
        _BDC[3].Data[2] = ui->lineEdit1_26->text().toInt()*10&0xFF;
        _BDC[3].Data[3] = (ui->lineEdit1_26->text().toInt()*10/256)&0xFF;       // 参与V2G次数
    }
    if (!ui->lineEdit1_27->text().isEmpty())
    {
        _BDC[3].Data[4] = ui->lineEdit1_27->text().toInt()*10&0xFF;
        _BDC[3].Data[5] = (ui->lineEdit1_27->text().toInt()*10/256)&0xFF;       // 参与V2G次数
    }
    // BSD parameter
    if (!ui->lineEdit1_18->text().isEmpty())
    {
        _BSD->Data[1] = ui->lineEdit1_18->text().toInt()&0xFF;
        _BSD->Data[2] = (ui->lineEdit1_18->text().toInt()/256)&0xFF;    // BSD 统计单体最高电压
    }
    if (!ui->lineEdit1_19->text().isEmpty())
    {
        _BSD->Data[3] = ui->lineEdit1_19->text().toInt()&0xFF;
        _BSD->Data[4] = (ui->lineEdit1_19->text().toInt()/256)&0xFF;    // BSD 统计单体最低电压
    }
    if (!ui->lineEdit1_20->text().isEmpty())
    {
        output = ui->lineEdit1_20->text();
        _BSD->Data[5] = uchar(processTemprature(output).at(0));         // BSD 统计单体最高温度
    }
    if (!ui->lineEdit1_21->text().isEmpty())
    {
        output = ui->lineEdit1_21->text();
        _BSD->Data[6] = uchar(processTemprature(output).at(0));         // BSD 统计单体最低温度
    }
    // BRM parameter
    if (!ui->lineEdit1_23->text().isEmpty())
    {
        _BRM[0].Data[5] = (ui->lineEdit1_23->text().toInt()*10)&0xFF;
        _BRM[0].Data[6] = (ui->lineEdit1_23->text().toInt()*10/256)&0xFF;
    }
    if (!ui->lineEdit1_24->text().isEmpty())
    {
        _BRM[0].Data[7] = (ui->lineEdit1_24->text().toInt()*10)&0xFF;
        _BRM[1].Data[1] = (ui->lineEdit1_24->text().toInt()*10/256)&0xFF;
    }
    // BCSP parameter
    if (!ui->lineEdit1_30->text().isEmpty())
    {
        _BCSP->Data[2] = (ui->lineEdit1_30->text().toInt()*10)&0xFF;
        _BCSP->Data[3] = (ui->lineEdit1_30->text().toInt()*10/256)&0xFF;
    }
    if (!ui->lineEdit1_29->text().isEmpty())
    {
        _BCSP->Data[0] = (ui->lineEdit1_29->text().toInt())&0xFF;
        _BCSP->Data[1] = (ui->lineEdit1_29->text().toInt()/256)&0xFF;
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
            _BDC[2].Data[1] = uchar(Vin_Code_Array.at(14));
            _BDC[2].Data[2] = uchar(Vin_Code_Array.at(15));
            _BDC[2].Data[3] = uchar(Vin_Code_Array.at(16));
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

QByteArray Widget::processCurrentBCLP(QString item, int k) // or can try str.toLatin1()/str.toUtf8()
{
    QByteArray dat;
    int val = 4000 + item.toInt() * k; // int < 2147483648
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
        int mod = (texttemp.length()/2);
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
            _BSM->Data[6] = 0xD0;
            qDebug() << "BMS had been setup to Normal Mode";
        }
    }
    else {
        if (stateMachine.state == H1 || stateMachine.state == S1)
        {
            stateMachine.state = V1; // V2G模式
            stateMachine.transform[16].action = N_A;
            stateMachine.transform[16].nextState = R2;
            _BSM->Data[6] = 0xC0;
            qDebug() << "BMS had been setup to V2G Mode";
        }
    }
}

void Widget::on_pushButton1_3_clicked()
{
    if(ui->checkBox1_2->isChecked())
    {
        stateMachine.state = H1; // 常规模式
        V2G_Mode_Flag = false;
        stateMachine.transform[16].action = BCL;
        stateMachine.transform[16].nextState = P1;
        _BSM->Data[6] = _BSM->Data[6] & 0xD0;           // 常规模式BSM默认允许充电
        qDebug() << "BMS had been setup to Normal Mode";
    }
    else {
        stateMachine.state = V1; // V2G模式
        V2G_Mode_Flag = true;
        stateMachine.transform[16].action = N_A;
        stateMachine.transform[16].nextState = R2;
        _BSM->Data[6] = _BSM->Data[6] & 0xC0;           // V2G模式BSM默认暂停后做CCD跟随
        qDebug() << "BMS had been setup to V2G Mode";
    }
    Charger_Info_init();
    runStateMachine(Start_Button);
}

void Widget::on_checkBox1_3_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[1].action = Busleep;     // BDC报文超时故障注入
    }
    else {
        stateMachine.transform[1].action = BDC;
    }
}

void Widget::on_checkBox1_5_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[2].action = Busleep;     // BHM报文超时故障注入
        stateMachine.transform[5].action = Busleep;
    }
    else {
        stateMachine.transform[2].action = BHM;
        stateMachine.transform[5].action = BHM;
    }
}

void Widget::on_checkBox1_4_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[11].action = Busleep;    // BRM报文超时故障注入
    }
    else {
        stateMachine.transform[11].action = BRM;
    }
}

void Widget::on_checkBox1_6_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[13].action = Busleep;    // BCP报文超时故障注入
    }
    else {
        stateMachine.transform[13].action = BCP;
    }
}

void Widget::on_checkBox1_7_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[14].action = Busleep;    // BR0_00报文超时故障注入
    }
    else {
        stateMachine.transform[14].action = BRO_00;
    }
}

void Widget::on_checkBox1_8_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[15].action = Busleep;    // BRO_AA报文超时故障注入
    }
    else {
        stateMachine.transform[15].action = BRO_AA;
    }
}

void Widget::on_checkBox1_9_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
//        _BCS->ID = 0x00000000;
        stateMachine.transform[19].action = N_A;    // BCS报文超时故障注入
        stateMachine.transform[25].action = N_A;
    }
    else {
//        _BCS->ID =0x1CEB56F4;
        stateMachine.transform[19].action = BCS;
        stateMachine.transform[25].action = BCS;
    }
}

void Widget::on_checkBox1_10_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        _BCL->ID= 0x00000000;    // BCL报文超时故障注入(修复帧ID)
        _BCLP->ID= 0x00000000;
        TimeoutBCL_Flag = true;
    }
    else {
        _BCL->ID= 0x181056F4;
        _BCLP->ID= 0x181056F4;
       TimeoutBCL_Flag = false;
    }
}

void Widget::on_checkBox1_11_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[23].action = Busleep;    // BST报文超时故障注入
        stateMachine.transform[29].action = Busleep;
    }
    else {
        stateMachine.transform[23].action = BST;
        stateMachine.transform[29].action = BST;
    }
}

void Widget::on_checkBox1_12_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        stateMachine.transform[37].action = Busleep;    // BSD报文超时故障注入
    }
    else {
        stateMachine.transform[37].action = BSD;
    }
}

void Widget::on_comboBox1_1_currentIndexChanged(const QString &arg1)
{
   _BRM[0].Data[4] = Battery_Type.value(arg1);
   qDebug() << "Set Battery_Type" << arg1 << _BRM[0].Data[4];
}

void Widget::on_comboBox1_2_currentIndexChanged(const QString &arg1)
{
    switch (BMS_Version_Set.value(arg1)) {
    case 0x01:
        _BRM[0].Data[2] = 0x01;
        _BRM[0].Data[3] = 0x00;
        break;
    case 0x02:
        _BRM[0].Data[2] = 0x00;
        _BRM[0].Data[3] = 0x00;
        break;
    case 0x03:
        _BRM[0].Data[2] = 0x01;
        _BRM[0].Data[3] = 0x02;
        break;
    default:
        break;
    }
    qDebug() << "BMS Version: " << _BRM[0].Data[2] << _BRM[0].Data[3];
}

void Widget::on_comboBox1_3_currentIndexChanged(const QString &arg1)
{
    _BCL->Data[4] = BCL_Mode_Flag.value(arg1);
    qDebug() << "Set BCL Mode" << _BCL->Data[4];
}

void Widget::on_checkBox1_13_stateChanged(int arg1)
{
    if (arg1==0x02)
    {
        _BSM->Data[6] = _BSM->Data[6] ^ 0x04;
    }
    else {
        _BSM->Data[6] = _BSM->Data[6] ^ 0x04;
    }
    qDebug() << _BSM->Data[6];
}

void Widget::on_checkBox1_14_stateChanged(int arg1)
{
    if (arg1==0x02)
    {
        _BSM->Data[6] = _BSM->Data[6] ^ 0x01;
    }
    else {
        _BSM->Data[6] = _BSM->Data[6] ^ 0x01;
    }
    qDebug() << _BSM->Data[6];
}

void Widget::on_checkBox1_16_stateChanged(int arg1)
{
    if (arg1==0x02)
    {
        _BSM->Data[5] = _BSM->Data[5] ^ 0x04;
        ui->checkBox1_15->setEnabled(false);
    }
    else {
        _BSM->Data[5] = _BSM->Data[5] ^ 0x04;
        ui->checkBox1_15->setEnabled(true);
    }
    qDebug() << _BSM->Data[5];
}

void Widget::on_checkBox1_15_stateChanged(int arg1)
{
    if (arg1==0x02)
    {
        _BSM->Data[5] = _BSM->Data[5] ^ 0x08;
        ui->checkBox1_16->setEnabled(false);
    }
    else {
        _BSM->Data[5] = _BSM->Data[5] ^ 0x08;
        ui->checkBox1_16->setEnabled(true);
    }
    qDebug() << _BSM->Data[5];
}

void Widget::Charger_Info_init()
{
    QString TempString = "F";
    ui->label1_6->setText(TempString);
    ui->label1_12->setText(TempString);
    ui->label1_19->setText(TempString);
    ui->label1_21->setText(TempString);
    ui->label1_23->setText(TempString);
    ui->label1_24->setText(TempString);
    ui->label1_25->setText(TempString);
    ui->label1_26->setText(TempString);
    ui->label1_27->setText(TempString);
    ui->label1_28->setText(TempString);
    ui->label1_29->setText(TempString);
    ui->lcdNumber1_1->display(0);
    ui->lcdNumber1_2->display(0);
    ui->label1_48->setText("Standby");
}

void Widget::on_comboBox1_4_currentIndexChanged(const QString &arg1)
{
    work_Channel = USBCAN_ChFlag.value(arg1); // work_Channel 作为接收通道0x00, 0x01
    gChMask = work_Channel + 0x01;            // gChMask 初始0x03作为两通道初始化, 后面赋值变化作为发送通道0x01, 0x02;
    qDebug() << "Now work work_Channel is " << work_Channel << "gChMask is" << gChMask;
}

void Widget::on_checkBox1_18_stateChanged(int arg1)
{
    if (arg1 == 0x02)
    {
        Fault_State_ACheck();
        if (_BSM->Data[6] == 0xD0)
        {
            ui->checkBox1_17->setCheckState(Qt::Checked);
        }
        else if (_BSM->Data[6] == 0xE0) {
            ui->checkBox1_1->setCheckState(Qt::Checked);
        }
    }
    else {
        if (ui->checkBox1_2->isChecked())
        {
            _BSM->Data[6] = 0xD0;   // 暂停状态恢复
        }
        else {
            _BSM->Data[6] = 0xC0;   // 暂停状态恢复
        }
        ui->checkBox1_17->setCheckState(Qt::Unchecked);
        ui->checkBox1_1->setCheckState(Qt::Unchecked);
        Fault_State_AUncheck();
    }
}

void Widget::on_checkBox1_17_clicked()
{
    if (ui->checkBox1_17->isChecked())
    {
        ui->checkBox1_1->setEnabled(false);
    }
    else {
        ui->checkBox1_1->setEnabled(true);
    }
    _BSM->Data[6] = _BSM->Data[6] ^ 0x10;       // 循环异或产生位开关效果
    qDebug() << "BMS request a charging state " << _BSM->Data[6] ;
}

void Widget::on_checkBox1_1_clicked()
{
    if (ui->checkBox1_1->isChecked())
    {
        ui->checkBox1_17->setEnabled(false);
    }
    else {
        ui->checkBox1_17->setEnabled(true);
    }
    _BSM->Data[6] = _BSM->Data[6] ^ 0x20;       // 循环异或产生位开关效果
    qDebug() << "BMS request a pause recharge state" << _BSM->Data[6];
}

void Widget::Fault_State_ACheck()
{
    ui->checkBox1_17->setEnabled(true);
    ui->checkBox1_16->setEnabled(true);
    ui->checkBox1_15->setEnabled(true);
    ui->checkBox1_14->setEnabled(true);
    ui->checkBox1_13->setEnabled(true);
    ui->checkBox1_12->setEnabled(true);
    ui->checkBox1_11->setEnabled(true);
    ui->checkBox1_10->setEnabled(true);
    ui->checkBox1_9->setEnabled(true);
    ui->checkBox1_8->setEnabled(true);
    ui->checkBox1_7->setEnabled(true);
    ui->checkBox1_6->setEnabled(true);
    ui->checkBox1_5->setEnabled(true);
    ui->checkBox1_4->setEnabled(true);
    ui->checkBox1_3->setEnabled(true);
    ui->checkBox1_1->setEnabled(true);
    ui->checkBox1_19->setEnabled(true);
}

void Widget::Fault_State_AUncheck()
{
    ui->checkBox1_17->setEnabled(false);
    ui->checkBox1_16->setEnabled(false);
    ui->checkBox1_15->setEnabled(false);
    ui->checkBox1_14->setEnabled(false);
    ui->checkBox1_13->setEnabled(false);
    ui->checkBox1_12->setEnabled(false);
    ui->checkBox1_11->setEnabled(false);
    ui->checkBox1_10->setEnabled(false);
    ui->checkBox1_9->setEnabled(false);
    ui->checkBox1_8->setEnabled(false);
    ui->checkBox1_7->setEnabled(false);
    ui->checkBox1_6->setEnabled(false);
    ui->checkBox1_5->setEnabled(false);
    ui->checkBox1_4->setEnabled(false);
    ui->checkBox1_3->setEnabled(false);
    ui->checkBox1_1->setEnabled(false);
    ui->checkBox1_19->setEnabled(false);
}

void Widget::on_pushButton2_1_clicked()
{
    // 未知错误
    if (ui->checkBox2_29->isChecked())
    {
        _BST->Data[0] = 0x00;
    }
    else {
        _BST->Data[0] = 0x40;
    }
    // SOC达到设置值
    if (ui->checkBox2_1->isChecked())
    {
        _BST->Data[0] |= 0x01;
    }
    else {
        _BST->Data[0] &= (~0x01);
    }
    // 总电压达到设置值
    if (ui->checkBox2_2->isChecked())
    {
        _BST->Data[0] |= (0x01<<2);
    }
    else {
        _BST->Data[0] &= (~(0x01<<2));
    }
    // 单体电压达到设置值
    if (ui->checkBox2_3->isChecked())
    {
        _BST->Data[0] |= (0x01<<4);
    }
    else {
        _BST->Data[0] &= (~(0x01<<4));
    }
    // 充电机主动中止设置, 优先级高于未知错误(串行)
    if (ui->checkBox2_4->isChecked())
    {
        _BST->Data[0] |= (0x01<<6);
    }
    else {
        _BST->Data[0] &= (~(0x01<<6));
    }
    // 绝缘故障
    if (ui->checkBox2_5->isChecked())
    {
        _BST->Data[1] |= 0x01;
    }
    else {
        _BST->Data[1] &= (~0x01);
    }
    // 连接器过温故障
    if (ui->checkBox2_6->isChecked())
    {
        _BST->Data[1] |= (0x01<<2);
    }
    else {
        _BST->Data[1] &= (~(0x01<<2));
    }
    // BMS元件过温
    if (ui->checkBox2_7->isChecked())
    {
        _BST->Data[1] |= (0x01<<4);
    }
    else {
        _BST->Data[1] &= (~(0x01<<4));
    }
    // 连接器故障
    if (ui->checkBox2_8->isChecked())
    {
        _BST->Data[1] |= (0x01<<6);
    }
    else {
        _BST->Data[1] &= (~(0x01<<6));
    }
    // 电池组过温故障
    if (ui->checkBox2_9->isChecked())
    {
        _BST->Data[2] |= 0x01;
    }
    else {
        _BST->Data[2] &= (~0x01);
    }
    // 高压继电器故障
    if (ui->checkBox2_10->isChecked())
    {
        _BST->Data[2] |= (0x01<<2);
    }
    else {
        _BST->Data[2] &= (~(0x01<<2));
    }
    // 检测点2电压异常
    if (ui->checkBox2_11->isChecked())
    {
        _BST->Data[2] |= (0x01<<4);
    }
    else {
        _BST->Data[2] &= (~(0x01<<4));
    }
    // 其他故障
    if (ui->checkBox2_12->isChecked())
    {
        _BST->Data[2] |= (0x01<<6);
    }
    else {
        _BST->Data[2] &= (~(0x01<<6));
    }
    // 电流过大
    if (ui->checkBox2_13->isChecked())
    {
        _BST->Data[3] |= 0x01;
    }
    else {
        _BST->Data[3] &= (~0x01);
    }
    // 电压异常
    if (ui->checkBox2_14->isChecked())
    {
        _BST->Data[3] |= (0x01<<2);
    }
    else {
        _BST->Data[3] &= (~(0x01<<2));
    }
    qDebug() << "Set finish charge BST" << _BST->Data[0];
    runStateMachine(Kill_Button);
}

void Widget::on_pushButton2_2_clicked()
{
    // CRM00 报文超时
    if (ui->checkBox2_21->isChecked())
    {
        _BEM->Data[0] |= 0x01;
    }
    else {
        _BEM->Data[0] &= (~0x01);
    }
    // CRMAA 报文超时
    if (ui->checkBox2_22->isChecked())
    {
        _BEM->Data[0] |= (0x01<<2);
    }
    else {
        _BEM->Data[0] &= (~(0x01<<2));
    }
    // CML超时
    if (ui->checkBox2_23->isChecked())
    {
        _BEM->Data[1] |= 0x01;
    }
    else {
        _BEM->Data[1] &= (~0x01);
    }
    // CRO超时
    if (ui->checkBox2_24->isChecked())
    {
        _BEM->Data[1] |= (0x01<<2);
    }
    else {
        _BEM->Data[1] &= (~(0x01<<2));
    }
    // CCS超时
    if (ui->checkBox2_25->isChecked())
    {
        _BEM->Data[2] |= 0x01;
    }
    else {
        _BEM->Data[2] &= (~0x01);
    }
    // CST超时
    if (ui->checkBox2_26->isChecked())
    {
        _BEM->Data[2] |= (0x01<<2);
    }
    else {
        _BEM->Data[2] &= (~(0x01<<2));
    }
    // CSD超时
    if (ui->checkBox2_27->isChecked())
    {
        _BEM->Data[3] |= 0x01;
    }
    else {
        _BEM->Data[3] &= (~0x01);
    }
    // 其他超时
    if (ui->checkBox2_28->isChecked())
    {
        _BEM->Data[3] |= (0x01<<2);
    }
    else {
        _BEM->Data[3] &= (~(0x01<<2));
    }
    runStateMachine(Null_6);
}

void Widget::on_checkBox2_15_clicked()
{
    if (ui->checkBox2_15->isChecked())
    {
        ui->checkBox2_16->setEnabled(false);
    }
    else {
        ui->checkBox2_16->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x01;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox2_16_clicked()
{
    if (ui->checkBox2_16->isChecked())
    {
        ui->checkBox2_15->setEnabled(false);
    }
    else {
        ui->checkBox2_15->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x02;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox2_17_clicked()
{
    if (ui->checkBox2_17->isChecked())
    {
        ui->checkBox2_18->setEnabled(false);
    }
    else {
        ui->checkBox2_18->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x10;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox2_18_clicked()
{
    if (ui->checkBox2_18->isChecked())
    {
        ui->checkBox2_17->setEnabled(false);
    }
    else {
        ui->checkBox2_17->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x20;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox2_19_clicked()
{
    if (ui->checkBox2_19->isChecked())
    {
        ui->checkBox2_20->setEnabled(false);
    }
    else {
        ui->checkBox2_20->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x40;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox2_20_clicked()
{
    if (ui->checkBox2_20->isChecked())
    {
        ui->checkBox2_19->setEnabled(false);
    }
    else {
        ui->checkBox2_19->setEnabled(true);
    }
    _BSM->Data[5] ^= 0x80;       // 循环异或产生位开关效果
    qDebug() << "Set BSM" << _BSM->Data[5];
}

void Widget::on_checkBox1_19_stateChanged(int arg1)
{
    if(arg1==0x02)
    {
        _BSM->ID= 0x00000000;    // BSM报文超时故障注入(修复帧ID)
    }
    else {
        _BSM->ID= 0x181356F4;
    }
}
