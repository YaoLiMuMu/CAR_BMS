#include "proceesframe.h"
#include "widget.h"

QString Widget::Gmesg = "CHM";
bool Widget::CRM_00 = 0;
bool Widget::CRM_AA = 0;
bool Widget::CML = 0;
short Widget::BCL2BCS = 0;
QByteArray Widget::Demand_CV = QByteArray::fromHex("A00F8F0101");//6810D80E01

proceesframe::proceesframe(QObject *parent) : QObject(parent)
{

}

void proceesframe::rx_thread()
{
    BYTE readiness_00[] = {0x00};
    BYTE readiness_AA[] = {0xAA};
    BYTE brm_ack[] = {0x11, 0x07, 0x01, 0xFF, 0xFF, 0x00, 0x02, 0x00};
    BYTE brm_conf[] = {0x13, 0x31, 0x00, 0x07, 0xFF, 0x00, 0x02, 0x00};
    BYTE bcp_ack[] = {0x11, 0x02, 0x01, 0xFF, 0xFF, 0x00, 0x06, 0x00};
    BYTE bcp_conf[] = {0x13, 0x0D, 0x00, 0x02, 0xFF, 0x00, 0x06, 0x00};
    BYTE bcs_ack[] = {0x11, 0x02, 0x01, 0xFF, 0xFF, 0x00, 0x11, 0x00};
    BYTE bcs_conf[] = {0x13, 0x09, 0x00, 0x02, 0xFF, 0x00, 0x11, 0x00};
    ReceTab rece_Tab[] = {
        {0x1826F456, CHM, 0xff, nullptr},
        {0x1801F456, CRM_00, 0x01, readiness_00},
        {0x1801F456, CRM_AA, 0x01, readiness_AA},
        {0x1CECF456, BRM_ACK, 0x8, brm_ack},
        {0x1CECF456, BRM_CONF, 0x08, brm_conf},
        {0x1CECF456, BCP_ACK, 0x08, bcp_ack},
        {0x1CECF456, BCP_CONF, 0x08, bcp_conf},
        {0x1CECF456, BCS_ACK, 0x08, bcs_ack},
        {0x1CECF456, BCS_CONF, 0x08, bcs_conf},
        {0x1808F456, CML, 0xff, nullptr},
        {0x100AF456, CRO_00, 0x01, readiness_00},
        {0x100AF456, CRO_AA, 0x01, readiness_AA}
    };
    qDebug() << "RX_thread ID: " << QThread::currentThreadId();
    VCI_ClearBuffer(gDevType, gDevIdx, 0);
    VCI_ClearBuffer(gDevType, gDevIdx, 1);
    RX_CTX * ctx = (RX_CTX *)malloc(sizeof(RX_CTX));    // 结构体指针必须初始化
    ctx->stop = 0;
    ctx->error = 0;
    ctx->channel = 1;
    ctx->total = 0; // reset counter
    VCI_CAN_OBJ can[RX_BUFF_SIZE]; // rececive buffer
    uint cnt; // current received number
    uint i,j;
    unsigned check_point = 0;
    while (!ctx->stop && !ctx->error)
    {
        cnt = VCI_Receive(gDevType, gDevIdx, ctx->channel, can, 1000, RX_WAIT_TIME); // 返回实际读取到的帧数
        if (!cnt)
        {
            VCI_ERR_INFO vei;
            if (VCI_ReadErrInfo(gDevType, gDevIdx, ctx->channel,&vei))
            {
                qDebug() << "Error code is : " << vei.ErrCode;
            }
            printf("Null Message receive\n");
            continue;
        }
        for (i = 0; i < cnt; i++) {

                QString Redata = QString::asprintf("CAN RX successed: ID=0x%08x, Data=0x", can[i].ID);
                for (j = 0; j < can[i].DataLen; j++)
                {
                    Redata = Redata + QString::asprintf("%02x", can[i].Data[j]);
                }
                qDebug() << Redata;
                for (int j = 0; j < 12; j++) {
                    if(can[i].ID == rece_Tab[j].Rece_CAN_ID && Verify_Frame(can[i], rece_Tab[j].Bit_num, rece_Tab[j].Data))
                    {
                        emit Send2Main(rece_Tab[j].Send_event);
                        qDebug() << "*******Event code is : " << rece_Tab[j].Send_event;
                        break;
                    }
                }
//                switch (can[i].ID) {
//                case 0x1826F456:
//                    Widget::Gmesg = "CHM";
//                    break;
//                case 0x1801F456:
//                    switch (can[i].Data[0]) {
//                    case 0x00:
//                        Widget::Gmesg = "CRM_00";
//                        Widget::CRM_00 = 1;
//                        break;
//                    case 0xaa:
//                        Widget::Gmesg = "CRM_aa";
//                        Widget::CRM_AA = 1;
//                        break;
//                    }
//                    break;
//                case 0x1CECF456:
//                    switch (can[i].Data[6]) {
//                    case 0x02:
//                        switch (can[i].Data[0]) {
//                        case 0x11:
//                            Widget::Gmesg = "CRM_re";
//                            break;
//                        case 0x13:
//                            if (Widget::CRM_AA)
//                               Widget::Gmesg = "CRM_aa";
//                            else {
//                                Widget::Gmesg = "CRM_00";
//                            }
//                            break;
//                        }
//                        break;
//                    case 0x06:
//                        switch (can[i].Data[0]) {
//                        case 0x11:
//                            Widget::Gmesg = "BCP_re";       // transmit BCP long_message
//                            break;
//                        case 0x13:
//                            if (Widget::CML)
//                                Widget::Gmesg = "BRO_aa";   // wait for CML after BCP, transmit BRO(0xAA)
//                            else {
//                                Widget::Gmesg = "BCP_re";
//                            }
//                        }
//                        break;
//                    case 0x11:
//                        switch (can[i].Data[0]) {
//                        case 0x11:
//                            Widget::Gmesg = "BCS_re";
//                            break;
//                        case 0x13:
//                            Widget::Gmesg = "BCL";
//                        }
//                    }
//                    break;
//                case 0x1808F456:
//                    Widget::Gmesg = "BRO_aa";   // after CML, transfer BRO(0xAA)
//                    Widget::CML = 1;            // stamp for CML history
//                    break;
//                case 0x100AF456:
//                    switch (can[i].Data[0]) {
//                    case 0x00:
//                        Widget::Gmesg = "BRO_aa";
//                        break;
//                    case 0xaa:
//                        Widget::Gmesg = "BCL";
//                    }
//                }
//                qDebug() << "****************************************" << Widget::Gmesg ;
                emit Sendcan(can[i]);
                continue;

            printf("CAN%d: verify_frame() failed\n", ctx->channel);
            ctx->error = 1;
            break;
        }
        if (ctx->error) break;

        ctx->total += cnt;
        if (ctx->total / CHECK_POINT >= check_point) {
            printf("CAN%d: %d frames received & verified\n", ctx->channel, ctx->total);
            check_point++;
        }
    }
    emit Send2UI(ctx->error);
}

int proceesframe::Verify_Frame(VCI_CAN_OBJ can, uint Len, BYTE * CAN_Data)
{
    if (Len == 0xff)
        return 1;
    for (uint i = 0; i< Len; i++) {
        if (can.Data[i] != CAN_Data[i])
            return 0;
    }
    return 1; // ext-frame ok
}
