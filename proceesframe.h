#ifndef PROCEESFRAME_H
#define PROCEESFRAME_H

#include <controlcan.h>
#include <QDebug>
#include <unistd.h>
#include <QObject>
#include <QThread>
#define msleep(ms)  usleep((ms)*1000)

typedef struct {
    unsigned channel; // channel index, 0~3
    unsigned stop; // stop RX-thread
    unsigned total; // total received
    unsigned error; // error(s) detected
} RX_CTX;

typedef struct{
    uint32_t Rece_CAN_ID;
    EventID Send_event;
    uint Bit_num;
    BYTE * Data;
} ReceTab;

class proceesframe : public QObject
{
    Q_OBJECT
public:
    explicit proceesframe(QObject *parent = nullptr);

signals:
    void Send2UI(unsigned);
    void Send2Main(EventID);
    void Sendcan(VCI_CAN_OBJ can);

public slots:
    int Verify_Frame(VCI_CAN_OBJ, uint, BYTE *);
    void rx_thread();

private:

};

#endif // PROCEESFRAME_H
