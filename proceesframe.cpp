#include "proceesframe.h"
#include "widget.h"

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
    BYTE bdc_ack[] = {0x11, 0x04, 0x01, 0xFF, 0xFF, 0x00, 0x29, 0x00};
    BYTE bdc_conf[] = {0x13, 0x1C, 0x00, 0x04, 0xFF, 0x00, 0x29, 0x00};
    BYTE ccd_00[] = {0x00};
    BYTE ccd_01[] = {0x01};
    ReceTab rece_Tab[] = {
        {0x1833F456, CSDP, 0xff, nullptr, 0},
        {0x1832F456, CCD_00, 0xff, ccd_00, 1},
        {0x1832F456, CCD_01, 0xff, ccd_01, 1},
        {0x1831F456, CMLP, 0xff, nullptr, 1},
        {0x1828F456, CDC, 0xff, nullptr, 0},
        {0x1CECF456, BDC_ACK, 0x08, bdc_ack, 0},
        {0x1CECF456, BDC_CONF, 0x08, bdc_conf, 0},
        {0x1826F456, CHM, 0xff, nullptr, 1},
        {0x1801F456, CRM_00, 0x01, readiness_00, 1},
        {0x1801F456, CRM_AA, 0x01, readiness_AA, 1},
        {0x1CECF456, BRM_ACK, 0x08, brm_ack, 0},
        {0x1CECF456, BRM_CONF, 0x08, brm_conf, 0},
        {0x1CECF456, BCP_ACK, 0x08, bcp_ack, 0},
        {0x1CECF456, BCP_CONF, 0x08, bcp_conf, 0},
        {0x1CECF456, BCS_ACK, 0x08, bcs_ack, 0},
        {0x1CECF456, BCS_CONF, 0x08, bcs_conf, 0},
        {0x1808F456, CML, 0xff, nullptr, 0},
        {0x100AF456, CRO_00, 0x01, readiness_00, 1},
        {0x100AF456, CRO_AA, 0x01, readiness_AA, 1},
        {0x1807F456, CTS, 0xff, nullptr, 1},
        {0x1812F456, CCS, 0xff, nullptr, 1},
        {0x101AF456, CST, 0xff, nullptr, 1},
        {0x181DF456, CSD, 0xff, nullptr, 1},
        {0x011FF456, CEM, 0xff, nullptr, 1}
    };
    int rece_Tab_num = 23;
    qDebug() << "RX_thread ID: " << QThread::currentThreadId();
    VCI_ClearBuffer(gDevType, gDevIdx, 0);
    VCI_ClearBuffer(gDevType, gDevIdx, 1);
    RX_CTX * ctx = (RX_CTX *)malloc(sizeof(RX_CTX));    // 结构体指针必须初始化
    ctx->stop = 0;
    ctx->error = 0;
    ctx->channel = work_Channel; // can channel 0 and 1
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
                qDebug() << "Error code is : " << vei.ErrCode;  // 查询usbCAN错误码定义
            }
            printf("Null Message receive\n");
            continue;
        }
        for (i = 0; i < cnt; i++) {
                QByteArray CAN_Array;
                QString Redata = QString::asprintf("CAN RX successed: ID=0x%08x, Data=0x", can[i].ID);
                for (j = 0; j < can[i].DataLen; j++)
                {
                    CAN_Array.data()[j] = can[i].Data[j];
                    Redata = Redata + QString::asprintf("%02x", can[i].Data[j]);
                }
                qDebug() << Redata;
                for (int j = 0; j < rece_Tab_num; j++) {
                    if(can[i].ID == rece_Tab[j].Rece_CAN_ID && Verify_Frame(can[i], rece_Tab[j].Bit_num, rece_Tab[j].Data))
                    {
                        emit Send2Main(rece_Tab[j].Send_event);
                        if (rece_Tab[j].Trans_flag)
                            emit Sendcan(rece_Tab[j].Send_event, CAN_Array);
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
    return 1; // Verify Frame is Ok
}
