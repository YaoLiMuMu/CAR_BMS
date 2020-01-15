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

class proceesframe : public QObject
{
    Q_OBJECT
public:
    explicit proceesframe(QObject *parent = nullptr);

signals:
    void Send2UI(unsigned);
    void Sendcan(VCI_CAN_OBJ can);

public slots:
    int verify_frame(VCI_CAN_OBJ *can);
    void rx_thread();

private:

};

#endif // PROCEESFRAME_H
