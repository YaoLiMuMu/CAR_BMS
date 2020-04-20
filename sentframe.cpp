#include "sentframe.h"
#include "widget.h"

sentframe::sentframe(QObject *parent) : QObject(parent)
{
    // BCL frame
//    MSG_BCL.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
//    MSG_BCL.car_frame->SendType = gTxType;
//    MSG_BCL.car_frame->RemoteFlag = 0;
//    MSG_BCL.car_frame->DataLen = 5;
//    MSG_BCL.car_frame->Data = Widget::_BCL;
//    MSG_BCL.car_frame->ID = 0x181056F4;
//    MSG_BCL.car_frame->ExternFlag = 1;
    MSG_BCL.car_frame = & Widget::_BCL[0];
    MSG_BCL.time_out = 1000;
    MSG_BCL.cycle_time = 50;
    MSG_BCL.len = 1;
    // BHM init frame
    MSG_BHM.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BHM.car_frame->SendType = gTxType;
    MSG_BHM.car_frame->RemoteFlag = 0;
    MSG_BHM.car_frame->DataLen = 2;
    MSG_BHM.car_frame->Data[0] = 0x4C;
    MSG_BHM.car_frame->Data[1] = 0x1D;
    MSG_BHM.car_frame->ID = 0x182756F4;
    MSG_BHM.car_frame->ExternFlag = 1;
    MSG_BHM.time_out = 5000;
    MSG_BHM.cycle_time = 250;
    MSG_BHM.len = 1;
    // BRO_00 frame
    MSG_BRO_00.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BRO_00.car_frame->SendType = gTxType;
    MSG_BRO_00.car_frame->RemoteFlag = 0;
    MSG_BRO_00.car_frame->DataLen = 1;
    MSG_BRO_00.car_frame->Data[0] = 0x00;
    MSG_BRO_00.car_frame->ID = 0x100956F4;
    MSG_BRO_00.car_frame->ExternFlag = 1;
    MSG_BRO_00.time_out = 5000;
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
    MSG_BRO_AA.time_out = 5000;
    MSG_BRO_AA.cycle_time = 250;
    MSG_BRO_AA.len = 1;
    // BRM 长消息起始帧
    MSG_BRM_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
//    memset(BRM_init.car_frame, 0, sizeof(VCI_CAN_OBJ));
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
    MSG_BRM_init.time_out = 5000;
    MSG_BRM_init.cycle_time = 0;
    MSG_BRM_init.len = 1;
    // BRM 长消息
    MSG_BRM.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*7);
//    memset(&BRM_con.car_frame[0], 0, sizeof(VCI_CAN_OBJ)*7);
    MSG_BRM.car_frame[0].SendType = gTxType;
    MSG_BRM.car_frame[0].RemoteFlag = 0;
    MSG_BRM.car_frame[0].DataLen = 8;
    MSG_BRM.car_frame[0].Data[0] = 0x01;
    MSG_BRM.car_frame[0].Data[1] = 0x01;
    MSG_BRM.car_frame[0].Data[2] = 0x01;
    MSG_BRM.car_frame[0].Data[3] = 0x00;
    MSG_BRM.car_frame[0].Data[4] = 0x01;
    MSG_BRM.car_frame[0].Data[5] = 0x88;
    MSG_BRM.car_frame[0].Data[6] = 0x13;
    MSG_BRM.car_frame[0].Data[7] = 0x4C;
    MSG_BRM.car_frame[0].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[0].ExternFlag = 1;
    MSG_BRM.car_frame[1].SendType = gTxType;
    MSG_BRM.car_frame[1].RemoteFlag = 0;
    MSG_BRM.car_frame[1].DataLen = 8;
    MSG_BRM.car_frame[1].Data[0] = 0x02;
    MSG_BRM.car_frame[1].Data[1] = 0x1D;
    MSG_BRM.car_frame[1].Data[2] = 0x4E;
    MSG_BRM.car_frame[1].Data[3] = 0x53;
    MSG_BRM.car_frame[1].Data[4] = 0x49;
    MSG_BRM.car_frame[1].Data[5] = 0x56;
    MSG_BRM.car_frame[1].Data[6] = 0x01;
    MSG_BRM.car_frame[1].Data[7] = 0x00;
    MSG_BRM.car_frame[1].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[1].ExternFlag = 1;
    MSG_BRM.car_frame[2].SendType = gTxType;
    MSG_BRM.car_frame[2].RemoteFlag = 0;
    MSG_BRM.car_frame[2].DataLen = 8;
    MSG_BRM.car_frame[2].Data[0] = 0x03;
    MSG_BRM.car_frame[2].Data[1] = 0x00;
    MSG_BRM.car_frame[2].Data[2] = 0x00;
    MSG_BRM.car_frame[2].Data[3] = 0x06;
    MSG_BRM.car_frame[2].Data[4] = 0x08;
    MSG_BRM.car_frame[2].Data[5] = 0x01;
    MSG_BRM.car_frame[2].Data[6] = 0x01;
    MSG_BRM.car_frame[2].Data[7] = 0x00;
    MSG_BRM.car_frame[2].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[2].ExternFlag = 1;
    MSG_BRM.car_frame[3].SendType = gTxType;
    MSG_BRM.car_frame[3].RemoteFlag = 0;
    MSG_BRM.car_frame[3].DataLen = 8;
    MSG_BRM.car_frame[3].Data[0] = 0x04;
    MSG_BRM.car_frame[3].Data[1] = 0x00;
    MSG_BRM.car_frame[3].Data[2] = 0x01;
    MSG_BRM.car_frame[3].Data[3] = 0xFF;
    MSG_BRM.car_frame[3].Data[4] = 'N';
    MSG_BRM.car_frame[3].Data[5] = 'Z';
    MSG_BRM.car_frame[3].Data[6] = '0';
    MSG_BRM.car_frame[3].Data[7] = '9';
    MSG_BRM.car_frame[3].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[3].ExternFlag = 1;
    MSG_BRM.car_frame[4].SendType = gTxType;
    MSG_BRM.car_frame[4].RemoteFlag = 0;
    MSG_BRM.car_frame[4].DataLen = 8;
    MSG_BRM.car_frame[4].Data[0] = 0x05;
    MSG_BRM.car_frame[4].Data[1] = '4';
    MSG_BRM.car_frame[4].Data[2] = '3';
    MSG_BRM.car_frame[4].Data[3] = 0x00;
    MSG_BRM.car_frame[4].Data[4] = 0x00;
    MSG_BRM.car_frame[4].Data[5] = 0x00;
    MSG_BRM.car_frame[4].Data[6] = 0x00;
    MSG_BRM.car_frame[4].Data[7] = 0x00;
    MSG_BRM.car_frame[4].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[4].ExternFlag = 1;
    MSG_BRM.car_frame[5].SendType = gTxType;
    MSG_BRM.car_frame[5].RemoteFlag = 0;
    MSG_BRM.car_frame[5].DataLen = 8;
    MSG_BRM.car_frame[5].Data[0] = 0x06;
    MSG_BRM.car_frame[5].Data[1] = 0x00;
    MSG_BRM.car_frame[5].Data[2] = 0x00;
    MSG_BRM.car_frame[5].Data[3] = 0x00;
    MSG_BRM.car_frame[5].Data[4] = 0x00;
    MSG_BRM.car_frame[5].Data[5] = 0x00;
    MSG_BRM.car_frame[5].Data[6] = 0x00;
    MSG_BRM.car_frame[5].Data[7] = 0xFF;
    MSG_BRM.car_frame[5].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[5].ExternFlag = 1;
    MSG_BRM.car_frame[6].SendType = gTxType;
    MSG_BRM.car_frame[6].RemoteFlag = 0;
    MSG_BRM.car_frame[6].DataLen = 8;
    MSG_BRM.car_frame[6].Data[0] = 0x07;
    MSG_BRM.car_frame[6].Data[1] = 0x0A;
    MSG_BRM.car_frame[6].Data[2] = 0x0B;
    MSG_BRM.car_frame[6].Data[3] = 0xDF;
    MSG_BRM.car_frame[6].Data[4] = 0x07;
    MSG_BRM.car_frame[6].Data[5] = 0xFF;
    MSG_BRM.car_frame[6].Data[6] = 0xFF;
    MSG_BRM.car_frame[6].Data[7] = 0xFF;
    MSG_BRM.car_frame[6].ID = 0x1CEB56F4;
    MSG_BRM.car_frame[6].ExternFlag = 1;
    MSG_BRM.time_out = 5000;
    MSG_BRM.cycle_time = 10;
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
    MSG_BCP_init.time_out = 5000;
    MSG_BCP_init.cycle_time = 1;
    MSG_BCP_init.len = 1;
    // BCP 长消息
    MSG_BCP.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*2);
//    memset(&MSG_BCP.car_frame[0], 0, sizeof(VCI_CAN_OBJ)*7);
    MSG_BCP.car_frame[0].SendType = gTxType;
    MSG_BCP.car_frame[0].RemoteFlag = 0;
    MSG_BCP.car_frame[0].DataLen = 8;
    MSG_BCP.car_frame[0].Data[0] = 0x01;
    MSG_BCP.car_frame[0].Data[1] = 0x60;
    MSG_BCP.car_frame[0].Data[2] = 0x09;
    MSG_BCP.car_frame[0].Data[3] = 0xDC;
    MSG_BCP.car_frame[0].Data[4] = 0x05;
    MSG_BCP.car_frame[0].Data[5] = 0x10;
    MSG_BCP.car_frame[0].Data[6] = 0x27;
    MSG_BCP.car_frame[0].Data[7] = 0x4C;
    MSG_BCP.car_frame[0].ID = 0x1CEB56F4;
    MSG_BCP.car_frame[0].ExternFlag = 1;
    MSG_BCP.car_frame[1].SendType = gTxType;
    MSG_BCP.car_frame[1].RemoteFlag = 0;
    MSG_BCP.car_frame[1].DataLen = 8;
    MSG_BCP.car_frame[1].Data[0] = 0x02;
    MSG_BCP.car_frame[1].Data[1] = 0x1D;
    MSG_BCP.car_frame[1].Data[2] = 0xFA;
    MSG_BCP.car_frame[1].Data[3] = 0xF4;
    MSG_BCP.car_frame[1].Data[4] = 0x01;
    MSG_BCP.car_frame[1].Data[5] = 0x4C;
    MSG_BCP.car_frame[1].Data[6] = 0x1D;
    MSG_BCP.car_frame[1].Data[7] = 0xFF;
    MSG_BCP.car_frame[1].ID = 0x1CEB56F4;
    MSG_BCP.car_frame[1].ExternFlag = 1;
    MSG_BCP.time_out = 5000;
    MSG_BCP.cycle_time = 10;
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
    MSG_BCS_init.time_out = 5000;
    MSG_BCS_init.cycle_time = 1;
    MSG_BCS_init.len = 1;
    // BCS 长消息
//    MSG_BCS.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*2);
//    MSG_BCS.car_frame[0].SendType = gTxType;
//    MSG_BCS.car_frame[0].RemoteFlag = 0;
//    MSG_BCS.car_frame[0].DataLen = 8;
//    MSG_BCS.car_frame[0].Data[0] = 0x01;
//    MSG_BCS.car_frame[0].Data[1] = 0x0F;
//    MSG_BCS.car_frame[0].Data[2] = 0x0F;
//    MSG_BCS.car_frame[0].Data[3] = 0x8E;
//    MSG_BCS.car_frame[0].Data[4] = 0x01;
//    MSG_BCS.car_frame[0].Data[5] = 0x98;
//    MSG_BCS.car_frame[0].Data[6] = 0x08;
//    MSG_BCS.car_frame[0].Data[7] = 0x32;
//    MSG_BCS.car_frame[0].ID = 0x1CEB56F4;
//    MSG_BCS.car_frame[0].ExternFlag = 1;
//    MSG_BCS.car_frame[1].SendType = gTxType;
//    MSG_BCS.car_frame[1].RemoteFlag = 0;
//    MSG_BCS.car_frame[1].DataLen = 8;
//    MSG_BCS.car_frame[1].Data[0] = 0x02;
//    MSG_BCS.car_frame[1].Data[1] = 0x2C;
//    MSG_BCS.car_frame[1].Data[2] = 0x01;
//    MSG_BCS.car_frame[1].Data[3] = 0xFF;
//    MSG_BCS.car_frame[1].Data[4] = 0xFF;
//    MSG_BCS.car_frame[1].Data[5] = 0xFF;
//    MSG_BCS.car_frame[1].Data[6] = 0xFF;
//    MSG_BCS.car_frame[1].Data[7] = 0xFF;
//    MSG_BCS.car_frame[1].ID = 0x1CEB56F4;
//    MSG_BCS.car_frame[1].ExternFlag = 1;
    MSG_BCS.car_frame = & Widget::_BCS[0];
    MSG_BCS.time_out = 5000;
    MSG_BCS.cycle_time = 10;
    MSG_BCS.len = 2;
    //  BSM frame
    MSG_BSM.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    MSG_BSM.car_frame[0].SendType = gTxType;
    MSG_BSM.car_frame[0].RemoteFlag = 0;
    MSG_BSM.car_frame->DataLen = 7;
    MSG_BSM.car_frame->Data[0] = 0x01;
    MSG_BSM.car_frame->Data[1] = 0x82;
    MSG_BSM.car_frame->Data[2] = 0x01;
    MSG_BSM.car_frame->Data[3] = 0x6E;
    MSG_BSM.car_frame->Data[4] = 0x02;
    MSG_BSM.car_frame->Data[5] = 0x00;
    MSG_BSM.car_frame->Data[6] = 0xD0;
    MSG_BSM.car_frame->ID = 0x181356F4;
    MSG_BSM.car_frame->ExternFlag = 1;
    MSG_BSM.time_out = 5000;
    MSG_BSM.cycle_time = 250;
    MSG_BSM.len = 1;
}



void sentframe::tx_thread(Action eve_act)
{
    qDebug() << "TX_thread ID: " << QThread::currentThreadId() << "execute action code is " << eve_act;
    // BCL frame
//    MSG_BCL.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
//    MSG_BCL.car_frame->SendType = gTxType;
//    MSG_BCL.car_frame->RemoteFlag = 0;
//    MSG_BCL.car_frame->DataLen = 5;
//    MSG_BCL.car_frame->Data[0] = uchar(Widget::Demand_CV.at(0));
//    MSG_BCL.car_frame->Data[1] = uchar(Widget::Demand_CV.at(1));
//    MSG_BCL.car_frame->Data[2] = uchar(Widget::Demand_CV.at(2));
//    MSG_BCL.car_frame->Data[3] = uchar(Widget::Demand_CV.at(3));
//    MSG_BCL.car_frame->Data[4] = uchar(Widget::Demand_CV.at(4));
//    MSG_BCL.car_frame->ID = 0x181056F4;
//    MSG_BCL.car_frame->ExternFlag = 1;
//    MSG_BCL.time_out = 1000;
//    MSG_BCL.cycle_time = 50;
//    MSG_BCL.len = 1;
//    // Qmap insert list
//    translist.insert("CHM", BHM);
//    translist.insert("CRM_00", BRM_init);
//    translist.insert("CRM_re", BRM_con);
//    translist.insert("CRM_aa", MSG_BCP_init);
//    translist.insert("BCP_re", MSG_BCP);
//    translist.insert("BRO_aa", BRO);
//    translist.insert("BCL", BCL);
//    translist.insert("BCS_re", MSG_BCS);
//    translist.insert("BCS_in", MSG_BCS_init);
//    VCI_CAN_OBJ *buff = (VCI_CAN_OBJ *)malloc(sizeof(VCI_CAN_OBJ) * gTxFrames);
//    unsigned tx;
//    for (tx = 0; tx < gTxCount; tx++)
//    {
//        for (int i = 0; i < 4; i++)
//        {
//            if ((gChMask & (1 << i)) == 0) continue;
//            while (translist.contains(Widget::Gmesg))
//            {
//                for (int j = 0; j < translist.value(Widget::Gmesg).len; j++)
//                {
//                    QDateTime nowtime = QDateTime::currentDateTime();
//                    QString timeblock = nowtime.toString("hh:mm:ss.zzz   ") + QString::asprintf("CAN TX successed: ID=0x%08x, Data=0x", translist.value(Widget::Gmesg).car_frame[j].ID);
//                    if( gTxFrames == VCI_Transmit(gDevType, gDevIdx, i, &translist.value(Widget::Gmesg).car_frame[j],  gTxFrames))
//                    {
//                        for (int ii = 0; ii < translist.value(Widget::Gmesg).car_frame[j].DataLen; ii++) {
//                            timeblock = timeblock + QString::asprintf("%02x", translist.value(Widget::Gmesg).car_frame[j].Data[ii]);
//                        }
//                        qDebug() << timeblock;
//                        msleep(translist.value(Widget::Gmesg).cycle_time);
//                    }
//                    else {
//                        j--;
//                        continue;
//                    }
//                    if (Widget::Gmesg == "CRM_re" && j == 6 && Widget::CRM_AA == 0)
//                    {
//                        Widget::Gmesg = "CRM_00";
////                        msleep(245);
//                    }
//                    if (Widget::Gmesg == "BCP_re" && j == 1 && Widget::CML == 0)
//                    {
//                        Widget::Gmesg = "CRM_aa";
////                        msleep(245);
//                    }
//                    if (Widget::Gmesg == "BCL")
//                    {
//                        BCL.car_frame->Data[0] = uchar(Widget::Demand_CV.at(0));
//                        BCL.car_frame->Data[1] = uchar(Widget::Demand_CV.at(1));
//                        BCL.car_frame->Data[2] = uchar(Widget::Demand_CV.at(2));
//                        BCL.car_frame->Data[3] = uchar(Widget::Demand_CV.at(3));
//                        BCL.car_frame->Data[4] = uchar(Widget::Demand_CV.at(4));
//                        Widget::BCL2BCS++;
//                        if (Widget::BCL2BCS == 6)
//                        {
//                            Widget::Gmesg = "BCS_in";
//                            Widget::BCL2BCS = 0;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    free(buff);
    switch (eve_act) {
    case BHM:
        tx_frame(MSG_BHM);
        //emit FeedBack(B_BHM);
        break;
    case BRM_INIT:
        tx_frame(MSG_BRM_init);
        break;
    case BRM:
        tx_frame(MSG_BRM);
        qDebug() << "MSG_BRM pointer address is " << MSG_BRM.car_frame;
        break;
    case BCP_INIT:
        tx_frame(MSG_BCP_init);
        qDebug() << "MSG_BCP_INIT pointer address is " << MSG_BCP_init.car_frame;
        break;
    case BCP:
        tx_frame(MSG_BCP);
        break;
    case BRO_00:
        tx_frame(MSG_BRO_00);
        msleep(40000);
        emit FeedBack(B_BRO_00);
        break;
    case BRO_AA:
        tx_frame(MSG_BRO_AA);
        break;
    case BCL:
        tx_frame(MSG_BCL);
        //emit FeedBack(B_BCL);
        //emit BCS_TimeStamp();
        break;
    case BCS_INIT:
        tx_frame(MSG_BCS_init);
        break;
    case BCS:
        tx_frame(MSG_BCS);
        break;
    }
}

void sentframe::generate_frame(VCI_CAN_OBJ * can)
{
    memset(can, 0, sizeof(VCI_CAN_OBJ));// can 赋值为0, 初始化
    can->SendType = gTxType;
    can->RemoteFlag = 0;
    can->DataLen = 2;
    can->Data[0] = 0x88;
    can->Data[1] = 0x13;
    can->ID = 0x182756F4;
    can->ExternFlag = 1;
}

void sentframe::tx_frame(CAN_Messages Msg)
{
    QTime startime = QTime::currentTime();
    for (uint j = 0; j < Msg.len; j++) {
        Auto_transmit(&Msg.car_frame[j]);
        QEventLoop loop;
        QTimer::singleShot(Msg.cycle_time, &loop, SLOT(quit()));
        loop.exec();
    }
    QTime endtime = QTime::currentTime();
    qDebug() << "This CAN frames spend time is " << startime.msecsTo(endtime) << "ms";
}

void sentframe::Auto_transmit(VCI_CAN_OBJ * vco)
{
    for (uint i = 0; i < 4; i++) {
        if ((gChMask & (1 << i)) == 0) {
            continue;
        }
        while (!VCI_Transmit(gDevType, gDevIdx, i, vco, 1)) {
            err++;
            continue;
        }
        QString Rece_log = QDateTime::currentDateTime().toString("hh:mm:ss.zzz   ") + QString::asprintf("CAN TX successed: ID=0x%08x, Data=0x", vco->ID);
        for (int ii = 0; ii < vco->DataLen; ii++) {
            Rece_log = Rece_log + QString::asprintf("%02x", vco->Data[ii]);
        }
        if (err == 256) // 2^8
        {
            qDebug() << "TX_transmit stop error : total errors times =" << err;
            emit Shoot_Error(err);
        }
        qDebug() << Rece_log;// QDateTime::currentMSecsSinceEpoch()表示ms总计时
    }
}
