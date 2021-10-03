#ifndef OZZLTCPSERVER_H
#define OZZLTCPSERVER_H

#include <QTcpServer>

class Q_NETWORK_EXPORT OzzlTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit OzzlTcpServer(QObject* pP = nullptr);

protected:
    void incomingConnection(qintptr) override;

private:
    QTcpServer* mpTcpServer;
};

#endif // OZZLTCPSERVER_H