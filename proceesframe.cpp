#include "proceesframe.h"
#include "widget.h"

QString Widget::Gmesg = "CHM";
bool Widget::CRM_00 = 0;
bool Widget::CRM_AA = 0;
bool Widget::CML = 0;
short Widget::BCL2BCS = 0;
QByteArray Widget::Demand_CV = QByteArray::fromHex("6810D80E01");

proceesframe::proceesframe(QObject *parent) : QObject(parent)
{

}

void proceesframe::rx_thread()
{
    qDebug() << "RXthread ID: " << QThread::currentThreadId();
    VCI_ClearBuffer(gDevType, gDevIdx, 0);
    VCI_ClearBuffer(gDevType, gDevIdx, 1);
    RX_CTX * ctx = (RX_CTX *)malloc(sizeof(RX_CTX));    // 结构体指针必须初始化
    ctx->stop = 0;
    ctx->error = 0;
    ctx->channel = 1;
    ctx->total = 0; // reset counter
    VCI_CAN_OBJ can[RX_BUFF_SIZE]; // buffer
    int cnt; // current received
    int i,j;
    unsigned check_point = 0;
    while (!ctx->stop && !ctx->error)
    {
        cnt = VCI_Receive(gDevType, gDevIdx, ctx->channel, can, 1000, RX_WAIT_TIME); // 返回实际读取到的帧数
        if (!cnt)
        {
            printf("Null Message receive\n");
            continue;
        }
        for (i = 0; i < cnt; i++) {
            if (verify_frame(&can[i]))
            {
                QString Redata = QString::asprintf("CAN RX successed: ID=0x%08x, Data=0x", can[i].ID);
                for (j = 0; j < can[i].DataLen; j++)
                {
                    Redata = Redata + QString::asprintf("%02x", can[i].Data[j]);
                }
                qDebug() << Redata;
                switch (can[i].ID) {
                case 0x1826F456:
                    Widget::Gmesg = "CHM";
                    break;
                case 0x1801F456:
                    switch (can[i].Data[0]) {
                    case 0x00:
                        Widget::Gmesg = "CRM_00";
                        Widget::CRM_00 = 1;
                        break;
                    case 0xaa:
                        Widget::Gmesg = "CRM_aa";
                        Widget::CRM_AA = 1;
                        break;
                    }
                    break;
                case 0x1CECF456:
                    switch (can[i].Data[6]) {
                    case 0x02:
                        switch (can[i].Data[0]) {
                        case 0x11:
                            Widget::Gmesg = "CRM_re";
                            break;
                        case 0x13:
                            if (Widget::CRM_AA)
                               Widget::Gmesg = "CRM_aa";
                            else {
                                Widget::Gmesg = "CRM_00";
                            }
                            break;
                        }
                        break;
                    case 0x06:
                        switch (can[i].Data[0]) {
                        case 0x11:
                            Widget::Gmesg = "BCP_re";
                            break;
                        case 0x13:
                            if (Widget::CML)
                                Widget::Gmesg = "BRO_aa";
                            else {
                                Widget::Gmesg = "BCP_re";
                            }
                        }
                        break;
                    case 0x11:
                        switch (can[i].Data[0]) {
                        case 0x11:
                            Widget::Gmesg = "BCS_re";
                            break;
                        case 0x13:
                            Widget::Gmesg = "BCL";
                        }
                    }
                    break;
                case 0x1808F456:
                    Widget::Gmesg = "BRO_aa";
                    Widget::CML = 1;
                    break;
                case 0x100AF456:
                    switch (can[i].Data[0]) {
                    case 0x00:
                        Widget::Gmesg = "BRO_aa";
                        break;
                    case 0xaa:
                        Widget::Gmesg = "BCL";
                    }
                }
                qDebug() << "****************************************" << Widget::Gmesg ;
                emit Sendcan(can[i]);
                continue;
            }
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

int proceesframe::verify_frame(VCI_CAN_OBJ *can)
{
    if (can->DataLen == 0 && can->DataLen > 8) return 0; // error: data length
    if (!can->ExternFlag) return 0; // std-frame ok
    return 1; // ext-frame ok
}
