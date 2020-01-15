#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    proceesframe * pframe = new proceesframe;
    sentframe * sframe = new sentframe;
    QThread * Tthread = new QThread;
    QThread * Rthread = new QThread;
    pframe->moveToThread(Rthread);
    sframe->moveToThread(Tthread);

    if (!VCI_OpenDevice(gDevType, gDevIdx, 0)) {
        printf("VCI_OpenDevice failed\n");
        return;
    }
    printf("VCI_OpenDevice succeeded\n");

    VCI_INIT_CONFIG config;
    config.AccCode = 0;
    config.AccMask = 0xffffffff;
    config.Filter = 1;  // 单滤波
    config.Mode = 0;    // 正常模式
    config.Timing0 = gBaud & 0xff;
    config.Timing1 = gBaud >> 8;

    for (int i = 0; i < MAX_CHANNELS; i++)
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

    connect(Tthread,SIGNAL(started()),sframe,SLOT(tx_thread()));
    connect(Rthread,SIGNAL(started()),pframe,SLOT(rx_thread()));
    Tthread->start();
    Rthread->start();

    msleep(3000);

    connect(pframe,SIGNAL(Send2UI(unsigned)),this,SLOT(CloseDev(unsigned)));
    qDebug() << "Mainthread ID: " << QThread::currentThreadId();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::CloseDev(unsigned Error)
{
    if (!Error)
        return;
    printf("TX/RX stopped, <ENTER> to CloseDevice...\n");
    getchar();
    VCI_ERR_INFO vei;
    if (!VCI_ReadErrInfo(gDevType, gDevIdx, 0, &vei))
        printf("Err_code=%0x\n", vei.ErrCode);
    VCI_CloseDevice(gDevType, gDevIdx);
    printf("VCI_CloseDevice\n");
}




void Widget::on_pushButton1_1_clicked()
{
    if (ui->lineEdit1_1->text().isEmpty() || ui->lineEdit1_2->text().isEmpty())
        {
        QMessageBox::information(this,"Warning","Please Input Voltage and Current");
        return;
    }
    QString output = ui->lineEdit1_1->text();
    QByteArray temp = processQString(output,10);
}

QByteArray Widget::processQString(QString item, int k) // or can try str.toLatin1()/str.toUtf8()
{
    QByteArray dat;
    int val = item.toInt() * k; // int < 2147483648
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
        dat.data()[i] = char((dat.data()[i] << 4) | ctempor);
    }
    return dat;
}
