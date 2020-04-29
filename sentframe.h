#ifndef SENTFRAME_H
#define SENTFRAME_H

#include <QObject>
#include <controlcan.h>
#include <QDebug>
#include <unistd.h>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QEventLoop>

typedef struct {
    VCI_CAN_OBJ * car_frame;
    unsigned len;
    unsigned cycle_time;
    unsigned time_out;
} CAN_Messages;

class sentframe : public QObject
{
    Q_OBJECT
public:
    explicit sentframe(QObject *parent = nullptr);

signals:
    void Shoot_Error(unsigned);
    void FeedBack(EventID);
    void BCS_TimeStamp();

public slots:
    void generate_frame(VCI_CAN_OBJ *);
    void tx_thread(Action);
    void tx_frame(CAN_Messages);
    void Auto_transmit(VCI_CAN_OBJ *);
private:
    QMap<QString, CAN_Messages> translist;
    QMutex m_mutex;
    uint err = 0;
    CAN_Messages MSG_BHM, MSG_BRM_init, MSG_BRM, MSG_BCP_init, MSG_BCP, MSG_BRO_00, MSG_BRO_AA, MSG_BCL, MSG_BCS_init, MSG_BCS, MSG_BSM;
    QTimer _threadIdleTimer;
};

#endif // SENTFRAME_H
