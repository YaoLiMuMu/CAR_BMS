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
    int cycle_time;
    bool Long_sign;
} CAN_Messages;

class sentframe : public QObject
{
    Q_OBJECT
public:
    explicit sentframe(QObject *parent = nullptr);

signals:
    void Shoot_Error(unsigned);
    void feedbackBRO_00();
    void feedbackBST();
    void finished();

public slots:
    void Initalize();
    void Loop_Send_Msg();
    void Trans_BSM();
    void tx_thread(Action);
    void tx_frame(CAN_Messages);
    void Auto_transmit(VCI_CAN_OBJ *);
private:
    uint err = 0;
    CAN_Messages MSG_BHM, MSG_BRM_init, MSG_BRM, MSG_BCP_init, MSG_BCP, MSG_BRO_00, MSG_BRO_AA, MSG_BCL, MSG_BCS_init, MSG_BCS, MSG_BSM,
                 MSG_BDC_init, MSG_BDC, MSG_BCPP, MSG_BCSP, MSG_BCLP, MSG_BST;
    QTimer *_BaseTimer;
    QTimer *_AnBaseTimer;
};

#endif // SENTFRAME_H
