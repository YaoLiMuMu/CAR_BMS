#ifndef CONTROLCAN_H
#define CONTROLCAN_H

//接口卡类型定义
#define VCI_PCI5121		1
#define VCI_PCI9810		2
#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_PCI9820		5
#define VCI_CAN232		6
#define VCI_PCI5110		7
#define VCI_CANLite		8
#define VCI_ISA9620		9
#define VCI_ISA5420		10

//CAN错误码
#define	ERR_CAN_OVERFLOW			0x0001	//CAN控制器内部FIFO溢出
#define	ERR_CAN_ERRALARM			0x0002	//CAN控制器错误报警
#define	ERR_CAN_PASSIVE				0x0004	//CAN控制器消极错误
#define	ERR_CAN_LOSE				0x0008	//CAN控制器仲裁丢失
#define	ERR_CAN_BUSERR				0x0010	//CAN控制器总线错误

//通用错误码
#define	ERR_DEVICEOPENED			0x0100	//设备已经打开
#define	ERR_DEVICEOPEN				0x0200	//打开设备错误
#define	ERR_DEVICENOTOPEN			0x0400	//设备没有打开
#define	ERR_BUFFEROVERFLOW			0x0800	//缓冲区溢出
#define	ERR_DEVICENOTEXIST			0x1000	//此设备不存在
#define	ERR_LOADKERNELDLL			0x2000	//装载动态库失败
#define ERR_CMDFAILED				0x4000	//执行命令失败错误码
#define	ERR_BUFFERCREATE			0x8000	//内存不足


//函数调用返回状态值
#define	STATUS_OK					1
#define STATUS_ERR					0
	
#define USHORT unsigned short int
#define BYTE unsigned char
#define CHAR char
#define UCHAR unsigned char
#define UINT unsigned int
#define DWORD unsigned int
#define PVOID void*
#define ULONG unsigned int
#define INT int
#define UINT32 UINT
#define LPVOID void*
#define BOOL BYTE
#define TRUE 1
#define FALSE 0


//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO{
		USHORT	hw_Version;
		USHORT	fw_Version;
		USHORT	dr_Version;
		USHORT	in_Version;
		USHORT	irq_Num;
		BYTE	can_Num;
		CHAR	str_Serial_Num[20];
		CHAR	str_hw_Type[40];
		USHORT	Reserved[4];
} VCI_BOARD_INFO,*PVCI_BOARD_INFO; 

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ{
	UINT	ID;
	UINT	TimeStamp;
	BYTE	TimeFlag;
	BYTE	SendType;
	BYTE	RemoteFlag;//是否是远程帧
	BYTE	ExternFlag;//是否是扩展帧
	BYTE	DataLen;
	BYTE	Data[8];
	BYTE	Reserved[3];
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.定义CAN控制器状态的数据类型。
typedef struct _VCI_CAN_STATUS{
	UCHAR	ErrInterrupt;
	UCHAR	regMode;
	UCHAR	regStatus;
	UCHAR	regALCapture;
	UCHAR	regECCapture; 
	UCHAR	regEWLimit;
	UCHAR	regRECounter; 
	UCHAR	regTECounter;
	DWORD	Reserved;
}VCI_CAN_STATUS,*PVCI_CAN_STATUS;

//4.定义错误信息的数据类型。
typedef struct _ERR_INFO{
		UINT	ErrCode;
		BYTE	Passive_ErrData[3];
		BYTE	ArLost_ErrData;
} VCI_ERR_INFO,*PVCI_ERR_INFO;

//5.定义初始化CAN的数据类型
typedef struct _INIT_CONFIG{
	DWORD	AccCode;
	DWORD	AccMask;
	DWORD	Reserved;
	UCHAR	Filter;
	UCHAR	Timing0;	
	UCHAR	Timing1;	
	UCHAR	Mode;
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

#ifdef __cplusplus
#define EXTERN_C  extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C DWORD VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);
EXTERN_C DWORD VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd);
EXTERN_C DWORD VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);

EXTERN_C DWORD VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);
EXTERN_C DWORD VCI_ReadErrInfo(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo);
EXTERN_C DWORD VCI_ReadCANStatus(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_STATUS pCANStatus);

EXTERN_C DWORD VCI_GetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);
EXTERN_C DWORD VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);

EXTERN_C ULONG VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERN_C DWORD VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERN_C DWORD VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERN_C DWORD VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERN_C ULONG VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,UINT Len);
EXTERN_C ULONG VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,UINT Len,INT WaitTime);

// ccustomszie define
#define MAX_CHANNELS  4
#define CHECK_POINT  200
#define RX_WAIT_TIME  100
#define RX_BUFF_SIZE  1000

static const unsigned gDevType = 0x04;  // USBCAN-II/II+
static const unsigned gDevIdx = 0x00;   // Device Index = 0
extern unsigned gChMask;   // channel 2 normal, bit0-CAN1, bit1-CAN2, 3=CAN1+CAN2

static const unsigned gBaud = 0x1C01;   // 250kb/s:1C01, 125kb/s:1C03
static const unsigned gTxType = 0x01;   // 0-normal, 1-single, 2-self_test
static const unsigned gTxSleep = 0x01;
static const unsigned gTxFrames = 0x01;
static const unsigned gTxCount = 0x01;
extern unsigned work_Channel; // receive frame channel ID, can channel 0 and 1


typedef enum {
    V1=1,       // V1 V2G mode message state
    H1=2,       // H1 Handshake start state
    H2=3,       // H2 Handshake recognition state
    C1=4,       // C1 parameter Configuration state
    R0=5,       // R0 Readiness bypass state
    R1=6,       // Readiness charge state
    R2=7,       // Readiness discharge state
    P1=8,       // P1 Power charge state
    P2=9,       // P2 Power discharge state
    T1=10,      // T1 first Timeout
    T2=11,      // T2 second Timeout
    T3=12,      // T3 third Timeout
    E1=13,      // E1 Ending State
    S1=14       // S1 System Sleep
} State;

typedef enum{
    Start_Button=1, // Press Start Button
    Kill_Button=2,  // Press End
    Ready_Button=3,   // Ready sigal
    CDC=4,
    CHM=5,          // receive CHM
    CRM_00=6,       // receive CRM(0x00)
    CRM_AA=7,       // receive CRM(0xAA)
    CTS=8,
    CML=9,
    CMLP=10,
    CRO_00=11,
    CRO_AA=12,
    CCD_00=13,
    CCD_01=14,
    CCS=15,
    CST=16,
    CSD=17,
    CSDP=18,
    CEM=19,
    BRM_ACK=20,
    BRM_CONF=21,
    BCP_ACK=22,
    BCP_CONF=23,
    BCS_ACK=24,
    BCS_CONF=25,
    BDC_ACK=26,
    BDC_CONF=27,
    Null_1,       // not receive CHM from system start stamp timeout=5s
    Null_2,       // not receive CRM from system start stamp timeout=60s
    Null_3,       // not receive CRM from CHM stamp timeout=30s
    Null_4,       // Null_4 not receive CRM from BEM stamp timeout=30s
    Null_5,       // Null_5 not receive frame(CDC/CHM) from system start stamp timeout=30s
    Null_6,       // Null_6 not receive CCS timeout=1s
    Null_7,       // Null_7 not receive CCD timeout=5s
} EventID;

typedef enum{
    BDC_INIT = 1,
    BDC = 2,
    BHM = 3,
    BRM_INIT = 4,
    BRM = 5,
    BCP_INIT = 6,
    BCP = 7,
    BCPP = 8,
    BRO_00 = 9,
    BRO_AA = 10,
    BCL = 11,
    BCLP = 12,
    BCS_INIT = 13,
    BCS = 14,
    BCSP = 15,
    BSM = 16,
    BMV = 17,
    BMT = 18,
    BSP = 19,
    BST = 20,
    BSD = 21,
    BEM_CRM = 22,
    BEM_CCD = 23,
    BEM_CCS = 24,
    Busleep = 25,
    N_A = 26,
    BEM,
} Action;

typedef struct {
    State curState;//当前状态
    EventID eventId;//事件ID
    State nextState;//下个状态
    Action action;//具体表现
} StateTransform;

#endif

