#ifndef OZZLTCPRWORKER_H
#define OZZLTCPRWORKER_H

#include "../common/ozzlprotocommon.h"

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>

class OzzlTcpWorker : public QObject
{
    Q_OBJECT

public:
    explicit OzzlTcpWorker(qintptr, QObject *pP = nullptr);

public slots:
    void slotInit();

private slots:
    void slotReqHandler();
    void slotOnDisconnect();

private:
    bool writeToSocket(QByteArray);
    bool sendResponse(Protocol::ResponseHeader*, double*, quint64);

    qintptr mSocketDescriptor;
    QTcpSocket* mpTcpSocket;
};

#endif // OZZLTCPRWORKER_H