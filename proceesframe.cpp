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
        {0x1833F456, CSDP, 0xff, nullptr, 1},
        {0x1832F456, CCD_00, 0x01, ccd_00, 1},
        {0x1832F456, CCD_01, 0x01, ccd_01, 1},
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
        {0x1808F456, CML, 0xff, nullptr, 1},
        {0x100AF456, CRO_00, 0x01, readiness_00, 0},
        {0x100AF456, CRO_AA, 0x01, readiness_AA, 0},
        {0x1807F456, CTS, 0xff, nullptr, 1},
        {0x1812F456, CCS, 0xff, nullptr, 1},
        {0x101AF456, CST, 0xff, nullptr, 1},
        {0x181DF456, CSD, 0xff, nullptr, 1},
        {0x081FF456, CEM, 0xff, nullptr, 1},
    };
    int rece_Tab_num = 23;
    qDebug() << "RX_thread ID: " << QThread::currentThreadId();
    VCI_ClearBuffer(gDevType, gDevIdx, work_Channel);
    RX_CTX * ctx = (RX_CTX *)malloc(sizeof(RX_CTX));    // 结构体指针必须初始化
    ctx->stop = 0;
    ctx->error = 0;
    // ctx->channel = work_Channel; // can channel 0 and 1,work_Channel设置在这里是为了更新界面设置的通道(或者通过重新线程实现)
    ctx->total = 0;             // reset counter
    VCI_CAN_OBJ can[RX_BUFF_SIZE]; // rececive buffer
    uint cnt;                   // current received number
    uint i,j;
    unsigned check_point = 0;
    while (!ctx->stop)          // while (!ctx->stop && !ctx->error)
    {
        ctx->channel = work_Channel; // can channel 0 and 1,work_Channel设置在这里是为了更新界面设置的通道(或者通过重启线程实现)
        cnt = VCI_Receive(gDevType, gDevIdx, ctx->channel, can, 1, RX_WAIT_TIME); // 返回实际读取到的帧数
        if (!cnt)
        {
            VCI_ERR_INFO vei;
            if (VCI_ReadErrInfo(gDevType, gDevIdx, ctx->channel,&vei))
            {
                if (vei.ErrCode != 0)
                    qDebug() << "CAN Receive Error code is : 0x" + QString::asprintf("%04x", vei.ErrCode);  // 查询usbCAN错误码定义
            }
            printf("Null Message receive\n");
            continue;
        }
        for (i = 0; i < cnt; i++) {
                QByteArray CAN_Array;
                CAN_Array.resize(can[i].DataLen);
                QString Redata = QString::asprintf("CAN RX successed: ID=0x%08x, Data=0x", can[i].ID);
                for (j = 0; j < can[i].DataLen; j++)
                {
                    CAN_Array.data()[j] = can[i].Data[j];
                    Redata = Redata + QString::asprintf("%02x", can[i].Data[j]);
                }
                qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + Redata;
                for (int j = 0; j < rece_Tab_num; j++) {
                    check_point = 1;
                    if(can[i].ID == rece_Tab[j].Rece_CAN_ID && Verify_Frame(can[i], rece_Tab[j].Bit_num, rece_Tab[j].Data))
                    {
                        check_point = 0;
                        emit Send2Main(rece_Tab[j].Send_event);
                        if (rece_Tab[j].Trans_flag)
                        {
                            emit Sendcan(rece_Tab[j].Send_event, CAN_Array);
                        }
                        qDebug() << "received and verified Event code is : " << rece_Tab[j].Send_event;
                        break;
                    }
                }
                ctx->error = ctx->error + check_point;
        }
        if (ctx->error > 256)
        {
            printf("Warning!!!CAN%d: has %d frames verify failed, I.e.these frames don't meet protocol\n、", ctx->channel, ctx->error);
        }
        ctx->total += cnt;
    }
    printf("CAN%d RX thread terminated, %d frames received & verified: %s\n",
            ctx->channel, ctx->total, ctx->error ? "error(s) detected" : "no error");
    if (ctx->error > 256)
        emit Send2UI(ctx->error);
    free(ctx);
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
