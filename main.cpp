#include "widget.h"
#include <QApplication>
#include <controlcan.h>

////***
//#ifdef WIN32 // for windows
//#   include <windows.h>
//#   include <process.h>
//#   include <stdio.h>
//#   include <time.h>
//#   include "controlcan.h"
//#   pragma comment(lib, "controlcan.lib")
//#   define msleep(ms)  Sleep(ms)
//typedef HANDLE pthread_t;
//#else // for linux
//#   include <stdio.h>
//#   include <stdlib.h>
//#   include <string.h>
//#   include <strings.h>
//#   include <unistd.h>
//#   include <sys/types.h>
//#   include <sys/stat.h>
//#   include <fcntl.h>
//#   include <pthread.h>
//#   include "controlcan.h"
//#   define msleep(ms)  usleep((ms)*1000)
//#   define min(a,b)  (((a) < (b)) ? (a) : (b)) // if (a<b); a; else b､
//#endif

//#define MAX_CHANNELS  4
//#define CHECK_POINT  200
//#define RX_WAIT_TIME  100
//#define RX_BUFF_SIZE  1000

//static unsigned gDevType = 0;
//static unsigned gDevIdx = 0;
//static unsigned gChMask = 0;
//static unsigned gBaud = 0;
//static unsigned gTxType = 0;
//static unsigned gTxSleep = 0;
//static unsigned gTxFrames = 0;
//static unsigned gTxCount = 0;
////***

////***
//void generate_frame(VCI_CAN_OBJ *can)
//{
//    memset(can, 0, sizeof(VCI_CAN_OBJ));// can 赋值为0, 初始化
//    can->SendType = gTxType;
//    can->RemoteFlag = 0;
//    can->DataLen = 2;
//    can->Data[0] = 0x13;
//    can->Data[1] = 0x88;
//    can->ID = 0x182756F4;
//    can->ExternFlag = 1;
//}

//int verify_frame(VCI_CAN_OBJ *can)
//{
//    if (can->DataLen == 0 && can->DataLen > 8) return 0; // error: data length
//    if (!can->ExternFlag) return 0; // std-frame ok
//    return 1; // ext-frame ok
//}

//typedef struct {
//    unsigned channel; // channel index, 0~3
//    unsigned stop; // stop RX-thread
//    unsigned total; // total received
//    unsigned error; // error(s) detected
//} RX_CTX;

//#ifdef WIN32
//unsigned __stdcall rx_thread(void *data)
//#else
//void * rx_thread(void *data)
//#endif
//{
//    RX_CTX *ctx = (RX_CTX *)data;
//    ctx->total = 0; // reset counter
//    VCI_CAN_OBJ can[RX_BUFF_SIZE]; // buffer
//    int cnt; // current received
//    printf("inital cnt=%x\n",cnt);
//    int i;
//    unsigned check_point = 0;
//    DWORD Rnum;
//    Rnum = VCI_GetReceiveNum(gDevType, gDevIdx, ctx->channel);
//    printf("gDevType=%0x,gDevIdx=%0x,channel=%0x,VCI_GetReceiveNum=%0x\n",gDevType, gDevIdx, ctx->channel,Rnum);
//    while (!ctx->stop && !ctx->error)
//    {
//        cnt = VCI_Receive(gDevType, gDevIdx, ctx->channel, can, RX_BUFF_SIZE, RX_WAIT_TIME); // 返回实际读取到的帧数
//        if (!cnt)
//        {
////            printf("Null Message receive\n");
//            continue;
//        }
//        printf("receive: %x\n",cnt);
//        for (i = 0; i < cnt; i++) {
//            if (verify_frame(&can[i]))
//                continue;
//            printf("CAN%d: verify_frame() failed\n", ctx->channel);
//            ctx->error = 1;
//            break;
//        }
//        if (ctx->error) break;

//        ctx->total += cnt;
//        if (ctx->total / CHECK_POINT >= check_point) {
//            printf("CAN%d: %d frames received & verified\n", ctx->channel, ctx->total);
//            check_point++;
//        }
//    }

//    printf("CAN%d RX thread terminated, %d frames received & verified: %s\n",
//        ctx->channel, ctx->total, ctx->error ? "error(s) detected" : "no error");

//#ifdef WIN32
//    _endthreadex(0);
//    return 0;
//#else
//    pthread_exit(0);
//#endif
//}

//int test()
//{
//    // ----- init & start -------------------------------------------------

//    VCI_INIT_CONFIG config;
//    config.AccCode = 0;
//    config.AccMask = 0xffffffff;
//    config.Filter = 1;  // 单滤波
//    config.Mode = 0;    // 正常模式
//    config.Timing0 = gBaud & 0xff;
//    config.Timing1 = gBaud >> 8;

//    int i, j;
//    for (i = 0; i < MAX_CHANNELS; i++)
//    {
//        if ((gChMask & (1 << i)) == 0) continue;

//        if (!VCI_InitCAN(gDevType, gDevIdx, i, &config))
//        {
//            printf("VCI_InitCAN(%d) failed\n", i);
//            return 0;
//        }
//        printf("VCI_InitCAN(%d) succeeded\n", i);

//        if (VCI_ClearBuffer(gDevType, gDevIdx, i))
//            printf("VCI_ClearBuffer(%d) succeeded\n", i);

//        if (!VCI_StartCAN(gDevType, gDevIdx, i))
//        {
//            printf("VCI_StartCAN(%d) failed\n", i);
//            return 0;
//        }
//        printf("VCI_StartCAN(%d) succeeded\n", i);
//    }

//    // ----- RX-timeout test ----------------------------------------------

//    VCI_CAN_OBJ can;
//    time_t tm1, tm2;
//    for (i = 0; i < 3; i++)
//    {
//        time(&tm1);
//        VCI_Receive(gDevType, gDevIdx, 0, &can, 1, (i + 1) * 1000/*ms*/);
//        time(&tm2);
//        printf("VCI_Receive returned: time ~= %ld seconds\n", tm2 - tm1);
//    }

//    // ----- create RX-threads --------------------------------------------

//    RX_CTX rx_ctx[MAX_CHANNELS];
//    pthread_t rx_threads[MAX_CHANNELS];
//    for (i = 0; i < MAX_CHANNELS; i++)
//    {
//        if ((gChMask & (1 << i)) == 0) continue;

//        rx_ctx[i].channel = i;
//        rx_ctx[i].stop = 0;
//        rx_ctx[i].total = 0;
//        rx_ctx[i].error = 0;
//#ifdef WIN32
//        rx_threads[i] = (HANDLE)_beginthreadex(NULL, 0, rx_thread, &rx_ctx[i], 0, NULL);
//#else
//        pthread_create(&rx_threads[i], NULL, rx_thread, &rx_ctx[i]);
//#endif
//    }

//    // ----- wait --------------------------------------------------------

//    printf("<ENTER> to start TX: %d*%d frames/channel, baud: t0=0x%02x, t1=0x%02x...\n",
//        gTxFrames, gTxCount, config.Timing0, config.Timing1);
//    getchar();

//    // ----- start transmit -----------------------------------------------

//    VCI_CAN_OBJ *buff = (VCI_CAN_OBJ *)malloc(sizeof(VCI_CAN_OBJ) * gTxFrames);
//    time(&tm1);
//    int err = 0;
//    unsigned tx;
//    for (tx = 0; !err && tx < gTxCount; tx++)
//    {
//        for (i = 0; i < MAX_CHANNELS; i++)
//        {
//            if ((gChMask & (1 << i)) == 0) continue;

//            for (j = 0; j < gTxFrames; j++)
//            {
//                generate_frame(&buff[j]);
//                printf("buff[0].ID:%08x, buff[0].Data:%08x ", buff[0].ID, buff[0].Data[0]);
//            }
//            if (gTxFrames != VCI_Transmit(gDevType, gDevIdx, i, &buff[0], gTxFrames))
//            {
//                printf("CAN%d TX failed: ID=%08x\n", i, can.ID);
//                err = 1;
//                break;
//            }
//            else {
//                printf("CAN%d TX successed: ID=%08x, Data=%08x\n", i, can.ID, can.Data);
//            }
//        }
//        if (gTxSleep) msleep(gTxSleep);
//    }
//    time(&tm2);
//    free(buff);

//    // ----- stop TX & RX -------------------------------------------------

//    msleep(1000);
//    printf("TX stopped, <ENTER> to terminate RX-threads...\n");
//    getchar();

//    for (i = 0; i < MAX_CHANNELS; i++)
//    {
//        if ((gChMask & (1 << i)) == 0) continue;

//        rx_ctx[i].stop = 1;
//#ifdef WIN32
//        WaitForSingleObject(rx_threads[i], INFINITE);
//        CloseHandle(rx_threads[i]);
//#else
//        pthread_join(rx_threads[i], NULL);
//#endif
//        if (rx_ctx[i].error)
//            err = 1;
//    }

//    // ----- report -------------------------------------------------------

//    if (err) {
//        printf("error(s) detected, test failed\n");
//        return 0;
//    }

//    printf("\n ***** %d frames/channel transferred, %ld seconds elapsed *****\n",
//        gTxFrames * gTxCount, tm2 - tm1);
//    if (tm2 - tm1)
//        printf("        performance: %ld frames/channel/second\n", gTxFrames * gTxCount / (tm2 - tm1));

//    return 1;
//}
//***



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    //***
//    gDevType = 0x04;
//    gDevIdx = 0x00;
//    gChMask = 0x01; // channel 2 normal
//    gBaud = 0x1C01;
//    gTxType = 0x03;
//    gTxSleep = 0x01;
//    gTxFrames = 0x01;
//    gTxCount = 0x01;
//    printf("DevType=%d, DevIdx=%d, ChMask=0x%x, Baud=0x%04x, TxType=%d, TxSleep=%d, TxFrames=0x%08x(%d), TxCount=0x%08x(%d)\n",
//        gDevType, gDevIdx, gChMask, gBaud, gTxType, gTxSleep, gTxFrames, gTxFrames, gTxCount, gTxCount);

//    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
//        printf("VCI_OpenDevice failed\n");
//        return 0;
//    }
//    printf("VCI_OpenDevice succeeded\n");
//    test();

//    VCI_ERR_INFO vei;
//    if (!VCI_ReadErrInfo(gDevType, gDevIdx, 0, &vei))
//        printf("Err_code=%0x\n", vei.ErrCode);
//    VCI_CloseDevice(gDevType, gDevIdx);
//    printf("VCI_CloseDevice\n");
//    return 0;
    //****

    return a.exec();
}




