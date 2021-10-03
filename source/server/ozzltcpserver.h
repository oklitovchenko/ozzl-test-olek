#ifndef OZZLTCPSERVER_H
#define OZZLTCPSERVER_H

#include <QTcpServer>
#include <QSocketNotifier>
#include <QMap>
#include <QThread>

class Q_NETWORK_EXPORT OzzlTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit OzzlTcpServer(QObject* pP = nullptr);

    // Unix signal handlers.
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);

public slots:
    // Qt signal handlers.
    void handleSigHup();
    void handleSigTerm();

protected:
    void incomingConnection(qintptr) override;

private:
    void onOsSigExit();

    QTcpServer* mpTcpServer;

    static int sighupFd[2];
    static int sigtermFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;

    bool mOsSigReceived = false;

    QMap<QThread*, bool> mIsThreadRunning;

private slots:
    void slotRemoveThread();
};

#endif // OZZLTCPSERVER_H