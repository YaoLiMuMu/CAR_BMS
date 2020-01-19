#include "sentframe.h"
#include "widget.h"

sentframe::sentframe(QObject *parent) : QObject(parent)
{

}

void sentframe::tx_thread()
{
    qDebug() << "TXthread ID: " << QThread::currentThreadId();
    FRAME BHM, BRM_init, BRM_con, BCP_init, BCP_con, BRO, BCL, BCS_init, BCS_con, BSM;
    // BCL frame
    BCL.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
    BCL.car_frame->SendType = gTxType;
    BCL.car_frame->RemoteFlag = 0;
    BCL.car_frame->DataLen = 5;
    BCL.car_frame->Data[0] = uchar(Widget::Demand_CV.at(0));
    BCL.car_frame->Data[1] = uchar(Widget::Demand_CV.at(1));
    BCL.car_frame->Data[2] = uchar(Widget::Demand_CV.at(2));
    BCL.car_frame->Data[3] = uchar(Widget::Demand_CV.at(3));
    BCL.car_frame->Data[4] = uchar(Widget::Demand_CV.at(4));
    BCL.car_frame->ID = 0x181056F4;
    BCL.car_frame->ExternFlag = 1;
    BCL.time_out = 1000;
    BCL.cycle_time = 50;
    BCL.len = 1;
    // BRO frame
    BRO.car_frame = (VCI_CAN_OBJ * ) malloc(sizeof (VCI_CAN_OBJ));
    BRO.car_frame->SendType = gTxType;
    BRO.car_frame->RemoteFlag = 0;
    BRO.car_frame->DataLen = 1;
    BRO.car_frame->Data[0] = 0xAA;
    BRO.car_frame->ID = 0x100956F4;
    BRO.car_frame->ExternFlag = 1;
    BRO.time_out = 5000;
    BRO.cycle_time = 250;
    BRO.len = 1;
    // BHM init frame
    BHM.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    BHM.car_frame->SendType = gTxType;
    BHM.car_frame->RemoteFlag = 0;
    BHM.car_frame->DataLen = 2;
    BHM.car_frame->Data[0] = 0x88;
    BHM.car_frame->Data[1] = 0x13;
    BHM.car_frame->ID = 0x182756F4;
    BHM.car_frame->ExternFlag = 1;
    BHM.time_out = 5000;
    BHM.cycle_time = 250;
    BHM.len = 1;
    // BRM 长消息起始帧
    BRM_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
//    memset(BRM_init.car_frame, 0, sizeof(VCI_CAN_OBJ));
    BRM_init.car_frame->SendType = gTxType;
    BRM_init.car_frame->RemoteFlag = 0;
    BRM_init.car_frame->DataLen = 8;
    BRM_init.car_frame->Data[0] = 0x10;
    BRM_init.car_frame->Data[1] = 0x31;
    BRM_init.car_frame->Data[2] = 0x00;
    BRM_init.car_frame->Data[3] = 0x07;
    BRM_init.car_frame->Data[4] = 0xFF;
    BRM_init.car_frame->Data[5] = 0x00;
    BRM_init.car_frame->Data[6] = 0x02;
    BRM_init.car_frame->Data[7] = 0x00;
    BRM_init.car_frame->ID = 0x1CEC56F4;
    BRM_init.car_frame->ExternFlag = 1;
    BRM_init.time_out = 5000;
    BRM_init.cycle_time = 5;
    BRM_init.len = 1;
    // BRM 长消息
    BRM_con.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*7);
//    memset(&BRM_con.car_frame[0], 0, sizeof(VCI_CAN_OBJ)*7);
    BRM_con.car_frame[0].SendType = gTxType;
    BRM_con.car_frame[0].RemoteFlag = 0;
    BRM_con.car_frame[0].DataLen = 8;
    BRM_con.car_frame[0].Data[0] = 0x01;
    BRM_con.car_frame[0].Data[1] = 0x01;
    BRM_con.car_frame[0].Data[2] = 0x01;
    BRM_con.car_frame[0].Data[3] = 0x00;
    BRM_con.car_frame[0].Data[4] = 0x01;
    BRM_con.car_frame[0].Data[5] = 0x88;
    BRM_con.car_frame[0].Data[6] = 0x13;
    BRM_con.car_frame[0].Data[7] = 0x88;
    BRM_con.car_frame[0].ID = 0x1CEB56F4;
    BRM_con.car_frame[0].ExternFlag = 1;
    BRM_con.car_frame[1].SendType = gTxType;
    BRM_con.car_frame[1].RemoteFlag = 0;
    BRM_con.car_frame[1].DataLen = 8;
    BRM_con.car_frame[1].Data[0] = 0x02;
    BRM_con.car_frame[1].Data[1] = 0x13;
    BRM_con.car_frame[1].Data[2] = 0x4E;
    BRM_con.car_frame[1].Data[3] = 0x53;
    BRM_con.car_frame[1].Data[4] = 0x49;
    BRM_con.car_frame[1].Data[5] = 0x56;
    BRM_con.car_frame[1].Data[6] = 0x01;
    BRM_con.car_frame[1].Data[7] = 0x00;
    BRM_con.car_frame[1].ID = 0x1CEB56F4;
    BRM_con.car_frame[1].ExternFlag = 1;
    BRM_con.car_frame[2].SendType = gTxType;
    BRM_con.car_frame[2].RemoteFlag = 0;
    BRM_con.car_frame[2].DataLen = 8;
    BRM_con.car_frame[2].Data[0] = 0x03;
    BRM_con.car_frame[2].Data[1] = 0x00;
    BRM_con.car_frame[2].Data[2] = 0x00;
    BRM_con.car_frame[2].Data[3] = 0x06;
    BRM_con.car_frame[2].Data[4] = 0x08;
    BRM_con.car_frame[2].Data[5] = 0x01;
    BRM_con.car_frame[2].Data[6] = 0x01;
    BRM_con.car_frame[2].Data[7] = 0x00;
    BRM_con.car_frame[2].ID = 0x1CEB56F4;
    BRM_con.car_frame[2].ExternFlag = 1;
    BRM_con.car_frame[3].SendType = gTxType;
    BRM_con.car_frame[3].RemoteFlag = 0;
    BRM_con.car_frame[3].DataLen = 8;
    BRM_con.car_frame[3].Data[0] = 0x04;
    BRM_con.car_frame[3].Data[1] = 0x00;
    BRM_con.car_frame[3].Data[2] = 0x01;
    BRM_con.car_frame[3].Data[3] = 0xFF;
    BRM_con.car_frame[3].Data[4] = 0xFF;
    BRM_con.car_frame[3].Data[5] = 0xFF;
    BRM_con.car_frame[3].Data[6] = 0xFF;
    BRM_con.car_frame[3].Data[7] = 0xFF;
    BRM_con.car_frame[3].ID = 0x1CEB56F4;
    BRM_con.car_frame[3].ExternFlag = 1;
    BRM_con.car_frame[4].SendType = gTxType;
    BRM_con.car_frame[4].RemoteFlag = 0;
    BRM_con.car_frame[4].DataLen = 8;
    BRM_con.car_frame[4].Data[0] = 0x05;
    BRM_con.car_frame[4].Data[1] = 0xFF;
    BRM_con.car_frame[4].Data[2] = 0xFF;
    BRM_con.car_frame[4].Data[3] = 0xFF;
    BRM_con.car_frame[4].Data[4] = 0xFF;
    BRM_con.car_frame[4].Data[5] = 0xFF;
    BRM_con.car_frame[4].Data[6] = 0xFF;
    BRM_con.car_frame[4].Data[7] = 0xFF;
    BRM_con.car_frame[4].ID = 0x1CEB56F4;
    BRM_con.car_frame[4].ExternFlag = 1;
    BRM_con.car_frame[5].SendType = gTxType;
    BRM_con.car_frame[5].RemoteFlag = 0;
    BRM_con.car_frame[5].DataLen = 8;
    BRM_con.car_frame[5].Data[0] = 0x06;
    BRM_con.car_frame[5].Data[1] = 0xFF;
    BRM_con.car_frame[5].Data[2] = 0xFF;
    BRM_con.car_frame[5].Data[3] = 0xFF;
    BRM_con.car_frame[5].Data[4] = 0xFF;
    BRM_con.car_frame[5].Data[5] = 0xFF;
    BRM_con.car_frame[5].Data[6] = 0xFF;
    BRM_con.car_frame[5].Data[7] = 0xFF;
    BRM_con.car_frame[5].ID = 0x1CEB56F4;
    BRM_con.car_frame[5].ExternFlag = 1;
    BRM_con.car_frame[6].SendType = gTxType;
    BRM_con.car_frame[6].RemoteFlag = 0;
    BRM_con.car_frame[6].DataLen = 8;
    BRM_con.car_frame[6].Data[0] = 0x07;
    BRM_con.car_frame[6].Data[1] = 0x0A;
    BRM_con.car_frame[6].Data[2] = 0x0B;
    BRM_con.car_frame[6].Data[3] = 0xDF;
    BRM_con.car_frame[6].Data[4] = 0x07;
    BRM_con.car_frame[6].Data[5] = 0xFF;
    BRM_con.car_frame[6].Data[6] = 0xFF;
    BRM_con.car_frame[6].Data[7] = 0xFF;
    BRM_con.car_frame[6].ID = 0x1CEB56F4;
    BRM_con.car_frame[6].ExternFlag = 1;
    BRM_con.time_out = 5000;
    BRM_con.cycle_time = 5;
    BRM_con.len = 7;
    // BCP 长消息起始帧
    BCP_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    BCP_init.car_frame->SendType = gTxType;
    BCP_init.car_frame->RemoteFlag = 0;
    BCP_init.car_frame->DataLen = 8;
    BCP_init.car_frame->Data[0] = 0x10;
    BCP_init.car_frame->Data[1] = 0x0D;
    BCP_init.car_frame->Data[2] = 0x00;
    BCP_init.car_frame->Data[3] = 0x02;
    BCP_init.car_frame->Data[4] = 0xFF;
    BCP_init.car_frame->Data[5] = 0x00;
    BCP_init.car_frame->Data[6] = 0x06;
    BCP_init.car_frame->Data[7] = 0x00;
    BCP_init.car_frame->ID = 0x1CEC56F4;
    BCP_init.car_frame->ExternFlag = 1;
    BCP_init.time_out = 5000;
    BCP_init.cycle_time = 5;
    BCP_init.len = 1;
    // BCP 长消息
    BCP_con.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*2);
//    memset(&BCP_con.car_frame[0], 0, sizeof(VCI_CAN_OBJ)*7);
    BCP_con.car_frame[0].SendType = gTxType;
    BCP_con.car_frame[0].RemoteFlag = 0;
    BCP_con.car_frame[0].DataLen = 8;
    BCP_con.car_frame[0].Data[0] = 0x01;
    BCP_con.car_frame[0].Data[1] = 0x60;
    BCP_con.car_frame[0].Data[2] = 0x09;
    BCP_con.car_frame[0].Data[3] = 0xB8;
    BCP_con.car_frame[0].Data[4] = 0x0B;
    BCP_con.car_frame[0].Data[5] = 0x10;
    BCP_con.car_frame[0].Data[6] = 0x27;
    BCP_con.car_frame[0].Data[7] = 0x88;
    BCP_con.car_frame[0].ID = 0x1CEB56F4;
    BCP_con.car_frame[0].ExternFlag = 1;
    BCP_con.car_frame[1].SendType = gTxType;
    BCP_con.car_frame[1].RemoteFlag = 0;
    BCP_con.car_frame[1].DataLen = 8;
    BCP_con.car_frame[1].Data[0] = 0x02;
    BCP_con.car_frame[1].Data[1] = 0x13;
    BCP_con.car_frame[1].Data[2] = 0xFA;
    BCP_con.car_frame[1].Data[3] = 0xF4;
    BCP_con.car_frame[1].Data[4] = 0x01;
    BCP_con.car_frame[1].Data[5] = 0xA0;
    BCP_con.car_frame[1].Data[6] = 0x0F;
    BCP_con.car_frame[1].Data[7] = 0xFF;
    BCP_con.car_frame[1].ID = 0x1CEB56F4;
    BCP_con.car_frame[1].ExternFlag = 1;
    BCP_con.time_out = 5000;
    BCP_con.cycle_time = 5;
    BCP_con.len = 2;
    //BCS 长消息起始帧
    BCS_init.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    BCS_init.car_frame[0].SendType = gTxType;
    BCS_init.car_frame[0].RemoteFlag = 0;
    BCS_init.car_frame->DataLen = 8;
    BCS_init.car_frame->Data[0] = 0x10;
    BCS_init.car_frame->Data[1] = 0x09;
    BCS_init.car_frame->Data[2] = 0x00;
    BCS_init.car_frame->Data[3] = 0x02;
    BCS_init.car_frame->Data[4] = 0xFF;
    BCS_init.car_frame->Data[5] = 0x00;
    BCS_init.car_frame->Data[6] = 0x11;
    BCS_init.car_frame->Data[7] = 0x00;
    BCS_init.car_frame->ID = 0x1CEC56F4;
    BCS_init.car_frame->ExternFlag = 1;
    BCS_init.time_out = 5000;
    BCS_init.cycle_time = 250;
    BCS_init.len = 1;
    // BCS 长消息
    BCS_con.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ)*2);
    BCS_con.car_frame[0].SendType = gTxType;
    BCS_con.car_frame[0].RemoteFlag = 0;
    BCS_con.car_frame[0].DataLen = 8;
    BCS_con.car_frame[0].Data[0] = 0x01;
    BCS_con.car_frame[0].Data[1] = 0xA0;
    BCS_con.car_frame[0].Data[2] = 0x0F;
    BCS_con.car_frame[0].Data[3] = 0xD8;
    BCS_con.car_frame[0].Data[4] = 0x0E;
    BCS_con.car_frame[0].Data[5] = 0x98;
    BCS_con.car_frame[0].Data[6] = 0x08;
    BCS_con.car_frame[0].Data[7] = 0x32;
    BCS_con.car_frame[0].ID = 0x1CEB56F4;
    BCS_con.car_frame[0].ExternFlag = 1;
    BCS_con.car_frame[1].SendType = gTxType;
    BCS_con.car_frame[1].RemoteFlag = 0;
    BCS_con.car_frame[1].DataLen = 8;
    BCS_con.car_frame[1].Data[0] = 0x02;
    BCS_con.car_frame[1].Data[1] = 0x2C;
    BCS_con.car_frame[1].Data[2] = 0xFF;
    BCS_con.car_frame[1].Data[3] = 0xFF;
    BCS_con.car_frame[1].Data[4] = 0xFF;
    BCS_con.car_frame[1].Data[5] = 0xFF;
    BCS_con.car_frame[1].Data[6] = 0xFF;
    BCS_con.car_frame[1].Data[7] = 0xFF;
    BCS_con.car_frame[1].ID = 0x1CEB56F4;
    BCS_con.car_frame[1].ExternFlag = 1;
    BCS_con.time_out = 5000;
    BCS_con.cycle_time = 5;
    BCS_con.len = 2;
    //  BSM frame
    BSM.car_frame = (VCI_CAN_OBJ *) malloc(sizeof (VCI_CAN_OBJ));
    BSM.car_frame[0].SendType = gTxType;
    BSM.car_frame[0].RemoteFlag = 0;
    BSM.car_frame->DataLen = 7;
    BSM.car_frame->Data[0] = 0x01;
    BSM.car_frame->Data[1] = 0x82;
    BSM.car_frame->Data[2] = 0x01;
    BSM.car_frame->Data[3] = 0x6E;
    BSM.car_frame->Data[4] = 0x02;
    BSM.car_frame->Data[5] = 0x00;
    BSM.car_frame->Data[6] = 0xD0;
    BSM.car_frame->ID = 0x181356F4;
    BSM.car_frame->ExternFlag = 1;
    BSM.time_out = 5000;
    BSM.cycle_time = 250;
    BSM.len = 1;
    // Qmap insert list
    translist.insert("CHM", BHM);
    translist.insert("CRM_00", BRM_init);
    translist.insert("CRM_re", BRM_con);
    translist.insert("CRM_aa", BCP_init);
    translist.insert("BCP_re", BCP_con);
    translist.insert("BRO_aa", BRO);
    translist.insert("BCL", BCL);
    translist.insert("BCS_re", BCS_con);
    translist.insert("BCS_in", BCS_init);
    VCI_CAN_OBJ *buff = (VCI_CAN_OBJ *)malloc(sizeof(VCI_CAN_OBJ) * gTxFrames);
//    time_t tm1, tm2;
//    time(&tm1);
    int err = 0;
    unsigned tx;
    for (tx = 0; !err && tx < gTxCount; tx++)
    {
        for (int i = 0; i < 4; i++)
        {
            if ((gChMask & (1 << i)) == 0) continue;

//            for (int j = 0; j < gTxFrames; j++)
//            {
//                generate_frame(&buff[j]);
//            }
            while (translist.contains(Widget::Gmesg))
            {
                for (int j = 0; j < translist.value(Widget::Gmesg).len; j++)
                {
                    QDateTime nowtime = QDateTime::currentDateTime();
                    QString timeblock = nowtime.toString("hh:mm:ss.zzz   ") + QString::asprintf("CAN TX successed: ID=0x%08x, Data=0x", translist.value(Widget::Gmesg).car_frame[j].ID);
                    if( gTxFrames == VCI_Transmit(gDevType, gDevIdx, i, &translist.value(Widget::Gmesg).car_frame[j],  gTxFrames))
                    {
                        for (int ii = 0; ii < translist.value(Widget::Gmesg).car_frame[j].DataLen; ii++) {
                            timeblock = timeblock + QString::asprintf("%02x", translist.value(Widget::Gmesg).car_frame[j].Data[ii]);
                        }
                        qDebug() << timeblock;
                        msleep(translist.value(Widget::Gmesg).cycle_time);
                    }
                    else {
                        j--;
                        continue;
                    }
                    if (Widget::Gmesg == "CRM_re" && j == 6 && Widget::CRM_AA == 0)
                    {
                        Widget::Gmesg = "CRM_00";
//                        msleep(245);
                    }
                    if (Widget::Gmesg == "BCP_re" && j == 1 && Widget::CML == 0)
                    {
                        Widget::Gmesg = "CRM_aa";
//                        msleep(245);
                    }
                    if (Widget::Gmesg == "BCL")
                    {
                        BCL.car_frame->Data[0] = uchar(Widget::Demand_CV.at(0));
                        BCL.car_frame->Data[1] = uchar(Widget::Demand_CV.at(1));
                        BCL.car_frame->Data[2] = uchar(Widget::Demand_CV.at(2));
                        BCL.car_frame->Data[3] = uchar(Widget::Demand_CV.at(3));
                        BCL.car_frame->Data[4] = uchar(Widget::Demand_CV.at(4));
                        Widget::BCL2BCS++;
                        if (Widget::BCL2BCS == 6)
                        {
                            Widget::Gmesg = "BCS_in";
                            Widget::BCL2BCS = 0;
                        }
                    }

        //                err = 1;
        //                break;
                }
            }
            qDebug() << QString::asprintf("buff[0].ID:%08x, buff[0].Data:%08x ", buff[0].ID, buff[0].Data[0]);
        }
        msleep(250);
    }
//    time(&tm2);
    free(buff);
}

void sentframe::generate_frame(VCI_CAN_OBJ *can)
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
