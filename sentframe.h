#ifndef SENTFRAME_H
#define SENTFRAME_H

#include <QObject>
#include <controlcan.h>
#include <QDebug>
#include <unistd.h>
#include <QThread>
#include <QDateTime>
#define msleep(ms)  usleep((ms)*1000)

typedef struct {
    VCI_CAN_OBJ * car_frame;
    unsigned len;
    unsigned cycle_time;
    unsigned time_out;
} FRAME;

class sentframe : public QObject
{
    Q_OBJECT
public:
    explicit sentframe(QObject *parent = nullptr);

signals:

public slots:
    void generate_frame(VCI_CAN_OBJ *can);
    void tx_thread();
private:
    QMap<QString, FRAME> translist;
};

#endif // SENTFRAME_H
