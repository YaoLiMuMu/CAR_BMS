#include "sentframe.h"
#include "widget.h"

sentframe::sentframe(QObject *parent) : QObject(parent)
{
    // BDC 长消息起始帧
    MSG_BDC_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    // memset(BRM_init.car_frame, 0, sizeof(VCI_CAN_OBJ));
    MSG_BDC_init.car_frame->SendType = gTxType;
    MSG_BDC_init.car_frame->RemoteFlag = 0;
    MSG_BDC_init.car_frame->DataLen = 8;
    MSG_BDC_init.car_frame->Data[0] = 0x10;
    MSG_BDC_init.car_frame->Data[1] = 0x1C;
    MSG_BDC_init.car_frame->Data[2] = 0x00;
    MSG_BDC_init.car_frame->Data[3] = 0x04;
    MSG_BDC_init.car_frame->Data[4] = 0xFF;
    MSG_BDC_init.car_frame->Data[5] = 0x00;
    MSG_BDC_init.car_frame->Data[6] = 0x29;
    MSG_BDC_init.car_frame->Data[7] = 0x00;
    MSG_BDC_init.car_frame->ID = 0x1CEC56F4;
    MSG_BDC_init.car_frame->ExternFlag = 1;
    MSG_BDC_init.Long_sign = 0;
    MSG_BDC_init.cycle_time = 250;
    MSG_BDC_init.len = 1;
    // BST frame
    MSG_BST.car_frame = & Widget::_BST[0];
    MSG_BST.Long_sign = 0;
    MSG_BST.cycle_time = 10;
    MSG_BST.len = 1;
    // BDC frame
    MSG_BDC.car_frame = & Widget::_BDC[0];
    MSG_BDC.Long_sign = 1;
    MSG_BDC.cycle_time = 0;
    MSG_BDC.len = 4;
    // BCPP frame
    MSG_BCPP.car_frame = & Widget::_BCPP[0];
    MSG_BCPP.Long_sign = 0;
    MSG_BCPP.cycle_time = 500;
    MSG_BCPP.len = 1;
    // BCSP frame
    MSG_BCSP.car_frame = & Widget::_BCSP[0];
    MSG_BCSP.Long_sign = 0;
    MSG_BCSP.cycle_time = 250;
    MSG_BCSP.len = 1;
    // BCL frame
    MSG_BCL.car_frame = & Widget::_BCL[0];
    MSG_BCL.Long_sign = 0;
    MSG_BCL.cycle_time = 50;
    MSG_BCL.len = 1;
    // BCLP frame
    MSG_BCLP.car_frame = & Widget::_BCLP[0];
    MSG_BCLP.Long_sign = 0;
    MSG_BCLP.cycle_time = 50;
    MSG_BCLP.len = 1;
    // BHM init frame
    MSG_BHM.car_frame = & Widget::_BHM[0];
    MSG_BHM.Long_sign = 0;
    MSG_BHM.cycle_time = 250;
    MSG_BHM.len = 1;
    // BRO_00 frame
    MSG_BRO_00.car_frame = (VCI_CAN_OBJ * ) malloc (sizeof (VCI_CAN_OBJ));
    MSG_BRO_00.car_frame->SendType = gTxType;
    MSG_BRO_00.car_frame->RemoteFlag = 0;
    MSG_BRO_00.car_frame->DataLen = 1;
    MSG_BRO_00.car_frame->Data[0] = 0x00;
    MSG_BRO_00.car_frame->ID = 0x100956F4;
    MSG_BRO_00.car_frame->ExternFlag = 1;
    MSG_BRO_00.Long_sign = 0;
    MSG_BRO_00.cycle_time = 250;
    MSG_BRO_00.len = 1;
    // BRO_AA frame
    MSG_BRO_AA.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BRO_AA.car_frame->SendType = gTxType;
    MSG_BRO_AA.car_frame->RemoteFlag = 0;
    MSG_BRO_AA.car_frame->DataLen = 1;
    MSG_BRO_AA.car_frame->Data[0] = 0xAA;
    MSG_BRO_AA.car_frame->ID = 0x100956F4;
    MSG_BRO_AA.car_frame->ExternFlag = 1;
    MSG_BRO_AA.Long_sign = 0;
    MSG_BRO_AA.cycle_time = 250;
    MSG_BRO_AA.len = 1;
    // BRM 长消息起始帧
    MSG_BRM_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BRM_init.car_frame->SendType = gTxType;
    MSG_BRM_init.car_frame->RemoteFlag = 0;
    MSG_BRM_init.car_frame->DataLen = 8;
    MSG_BRM_init.car_frame->Data[0] = 0x10;
    MSG_BRM_init.car_frame->Data[1] = 0x31;
    MSG_BRM_init.car_frame->Data[2] = 0x00;
    MSG_BRM_init.car_frame->Data[3] = 0x07;
    MSG_BRM_init.car_frame->Data[4] = 0xFF;
    MSG_BRM_init.car_frame->Data[5] = 0x00;
    MSG_BRM_init.car_frame->Data[6] = 0x02;
    MSG_BRM_init.car_frame->Data[7] = 0x00;
    MSG_BRM_init.car_frame->ID = 0x1CEC56F4;
    MSG_BRM_init.car_frame->ExternFlag = 1;
    MSG_BRM_init.Long_sign = 0;
    MSG_BRM_init.cycle_time = 250;
    MSG_BRM_init.len = 1;
    // BRM 长消息
    MSG_BRM.car_frame = & Widget::_BRM[0];
    MSG_BRM.Long_sign = 1;
    MSG_BRM.cycle_time = 0;
    MSG_BRM.len = 7;
    // BCP 长消息起始帧
    MSG_BCP_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BCP_init.car_frame->SendType = gTxType;
    MSG_BCP_init.car_frame->RemoteFlag = 0;
    MSG_BCP_init.car_frame->DataLen = 8;
    MSG_BCP_init.car_frame->Data[0] = 0x10;
    MSG_BCP_init.car_frame->Data[1] = 0x0D;
    MSG_BCP_init.car_frame->Data[2] = 0x00;
    MSG_BCP_init.car_frame->Data[3] = 0x02;
    MSG_BCP_init.car_frame->Data[4] = 0xFF;
    MSG_BCP_init.car_frame->Data[5] = 0x00;
    MSG_BCP_init.car_frame->Data[6] = 0x06;
    MSG_BCP_init.car_frame->Data[7] = 0x00;
    MSG_BCP_init.car_frame->ID = 0x1CEC56F4;
    MSG_BCP_init.car_frame->ExternFlag = 1;
    MSG_BCP_init.Long_sign = 0;
    MSG_BCP_init.cycle_time = 500;
    MSG_BCP_init.len = 1;
    // BCP 长消息
    MSG_BCP.car_frame = & Widget::_BCP[0];
    MSG_BCP.Long_sign = 1;
    MSG_BCP.cycle_time = 0;
    MSG_BCP.len = 2;
    //BCS 长消息起始帧
    MSG_BCS_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BCS_init.car_frame[0].SendType = gTxType;
    MSG_BCS_init.car_frame[0].RemoteFlag = 0;
    MSG_BCS_init.car_frame->DataLen = 8;
    MSG_BCS_init.car_frame->Data[0] = 0x10;
    MSG_BCS_init.car_frame->Data[1] = 0x09;
    MSG_BCS_init.car_frame->Data[2] = 0x00;
    MSG_BCS_init.car_frame->Data[3] = 0x02;
    MSG_BCS_init.car_frame->Data[4] = 0xFF;
    MSG_BCS_init.car_frame->Data[5] = 0x00;
    MSG_BCS_init.car_frame->Data[6] = 0x11;
    MSG_BCS_init.car_frame->Data[7] = 0x00;
    MSG_BCS_init.car_frame->ID = 0x1CEC56F4;
    MSG_BCS_init.car_frame->ExternFlag = 1;
    MSG_BCS_init.Long_sign = 0;
    MSG_BCS_init.cycle_time = 250;
    MSG_BCS_init.len = 1;
    // BCS 长消息
    MSG_BCS.car_frame = & Widget::_BCS[0];
    MSG_BCS.Long_sign = 1;
    MSG_BCS.cycle_time = 0;
    MSG_BCS.len = 2;
    //  BSM frame
    MSG_BSM.car_frame = & Widget::_BSM[0];
    MSG_BSM.Long_sign = 0;
    MSG_BSM.cycle_time = 250;
    MSG_BSM.len = 1;
    // BSD frameon_checkBox1_10_stateChanged
    MSG_BSD.car_frame = & Widget::_BSD[0];
    MSG_BSD.Long_sign = 0;
    MSG_BSD.cycle_time = 250;
    MSG_BSD.len = 1;
    // BEM frame
    MSG_BEM.car_frame = & Widget::_BEM[0];
    MSG_BEM.Long_sign = 0;
    MSG_BEM.cycle_time = 250;
    MSG_BEM.len = 1;
}

void sentframe::Initalize()
{
    qDebug() << "TX_thread ID: " << QThread::currentThreadId();
    _BaseTimer = new QTimer (this);
    _AnBaseTimer = new QTimer (this);
    connect(_BaseTimer, SIGNAL(timeout()), this, SLOT(Loop_Send_Msg()));
    connect(_AnBaseTimer, SIGNAL(timeout()), this, SLOT(Trans_BSM()));
    _BaseTimer->setTimerType(Qt::PreciseTimer);
    _AnBaseTimer->setTimerType(Qt::PreciseTimer);   // 定时器使用高精度类型
}

void sentframe::tx_thread(Action eve_act)
{
//    qDebug() << "TX_thread ID: " << QThread::currentThreadId() << "execute action code is " << eve_act;
    if (eve_act == N_A) // N_A switch state and keep can bus transmit
        return;
    if ((eve_act != BRM)&&(eve_act != BCS)&&(eve_act != BCP)&&(eve_act != BDC))     // 长消息帧发送时不打断原周期
    {
        _BaseTimer->stop();
        _AnBaseTimer->stop();
    }
    switch (eve_act) {
    case BHM:
        tx_frame(MSG_BHM);
        _BaseTimer->start(MSG_BHM.cycle_time);
        break;
    case BRM_INIT:
        tx_frame(MSG_BRM_init);
        _BaseTimer->start(MSG_BRM_init.cycle_time);
        break;
    case BRM:
        tx_frame(MSG_BRM);
        break;
    case BCP_INIT:
        tx_frame(MSG_BCP_init);
        if (Widget::V2G_Mode_Flag)
        {
            msleep(10);
            tx_frame(MSG_BCPP);                 // BCP and BCPP message transmit together
        }
        _BaseTimer->start(MSG_BCPP.cycle_time);
        break;
    case BCP:
        tx_frame(MSG_BCP);
        break;
    case BRO_00:
        tx_frame(MSG_BRO_00);
        emit feedbackBRO_00();
        _BaseTimer->start(MSG_BRO_00.cycle_time);
        break;
    case BSD:
        tx_frame(MSG_BSD);
        emit feedbackBST_BSD();
        _BaseTimer->start(MSG_BSD.cycle_time);
        break;
    case BST:
        tx_frame(MSG_BST);
        emit feedbackBST_BSD();
        _BaseTimer->start(MSG_BST.cycle_time);
        break;
    case BRO_AA:
        tx_frame(MSG_BRO_AA);
        _BaseTimer->start(MSG_BRO_AA.cycle_time);
        break;
    case BCL:
        tx_frame(MSG_BCL);
        _BaseTimer->start(MSG_BCL.cycle_time);
        tx_frame(MSG_BSM);
        if (Widget::V2G_Mode_Flag)
        {
            msleep(10);
            tx_frame(MSG_BCSP);
        }
        tx_frame(MSG_BCS_init);
        _AnBaseTimer->start(MSG_BSM.cycle_time);
        break;
    case BCLP:
        tx_frame(MSG_BCLP);
        _BaseTimer->start(MSG_BCLP.cycle_time);
        tx_frame(MSG_BSM);
        if (Widget::V2G_Mode_Flag)
        {
            msleep(10);
            tx_frame(MSG_BCSP);
        }
        tx_frame(MSG_BCS_init);
        _AnBaseTimer->start(MSG_BSM.cycle_time);
        break;
    case BCS_INIT:
        tx_frame(MSG_BCS_init);
        break;
    case BCS:
        tx_frame(MSG_BCS);
        break;
    case Busleep:
          // 总线休眠, 原先设置500ms会关闭总线, 与故障注入功能相关
        break;
    case BDC:
        tx_frame(MSG_BDC);
        break;
    case BDC_INIT:
        tx_frame(MSG_BDC_init);
        _BaseTimer->start(MSG_BDC_init.cycle_time);
        break;
    case BEM:
        tx_frame(MSG_BEM);
        _BaseTimer->start(MSG_BEM.cycle_time);
        break;
    default:
        break;
    }
}

void sentframe::Loop_Send_Msg()// Priodic message
{
    switch (stateMachine.state) {
    case V1:
        tx_frame(MSG_BDC_init);
        break;
    case H1:
        tx_frame(MSG_BHM);
        break;
    case H2:
        tx_frame(MSG_BRM_init);
        break;
    case C1:
        tx_frame(MSG_BCP_init);     // BCP and BCPP message transmit together
        if (Widget::V2G_Mode_Flag)
        {
            msleep(10);
            tx_frame(MSG_BCPP);
        }
        break;
    case P1:
        if (Widget::TimeoutBCL_Flag)
            tx_frame(MSG_BCL);
        break;
    case P2:
        tx_frame(MSG_BCLP);
        break;
    case R0:
        tx_frame(MSG_BRO_00);
        break;
    case R1:
        tx_frame(MSG_BRO_AA);
        break;
    case R2:
        tx_frame(MSG_BRO_AA);
        break;
    case E1:
        tx_frame(MSG_BST);
        break;
    case S1:
        tx_frame(MSG_BSD);
        break;
    case T1:
        tx_frame(MSG_BEM);
        break;
    case T2:
        tx_frame(MSG_BEM);
        break;
    case T3:
        tx_frame(MSG_BEM);
        break;
    default:
        break;
    }
}

void sentframe::Trans_BSM()     // another timer for BSM & BCS & BCSP
{
    switch (stateMachine.state) {
    case P1:
        tx_frame(MSG_BSM);
        msleep(10);
        tx_frame(MSG_BCSP);
        tx_frame(MSG_BCS_init);
        break;
    case P2:
        tx_frame(MSG_BSM);
        msleep(10);
        tx_frame(MSG_BCSP);
        tx_frame(MSG_BCS_init);
        break;
    default:
        break;
    }
}

void sentframe::tx_frame(CAN_Messages Msg)
{
//    QTime startime = QTime::currentTime();
    if (Widget::transFree)
        return;
    else {
        Widget::transFree = true;
    }
    if (!Msg.Long_sign)
    {
        Auto_transmit(&Msg.car_frame[0]);
    }
    else {
        for (uint j = 0; j < Msg.len; j++) {
            Auto_transmit(&Msg.car_frame[j]);
            QEventLoop loop;
            QTimer::singleShot(10, Qt::PreciseTimer, &loop, SLOT(quit()));    // 长消息报文帧间隔时间10ms, 高精度类型
            loop.exec();
        }
    }
    Widget::transFree = false;
//    QTime endtime = QTime::currentTime();
//    qDebug() << "This CAN frames spend time is " << startime.msecsTo(endtime) << "ms";
}

void sentframe::Auto_transmit(VCI_CAN_OBJ * vco)
{
    for (uint i = 0; i < 4; i++) {
        if ((gChMask & (1 << i)) == 0) {
            continue;
        } 
        m_mutex.lock(); // _BSM 全局变量互锁
        while (!VCI_Transmit(gDevType, gDevIdx, i, vco, 1)) {
            err++;
            continue;
        }
        m_mutex.unlock();
        QString Rece_log = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + QString::asprintf("CAN TX successed: ID=0x%08x, Data=0x", vco->ID);
        for (int ii = 0; ii < vco->DataLen; ii++) {
            Rece_log = Rece_log + QString::asprintf("%02x", vco->Data[ii]);
        }
        qDebug() << Rece_log;// QDateTime::currentMSecsSinceEpoch()表示ms总计时
        if (err == 256) // 2^8
        {
            qDebug() << "TX_transmit stop error : total errors times =" << err;
            emit Shoot_Error(err);
            emit finished();
        }
    }
}
