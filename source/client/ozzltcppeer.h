#ifndef OZZLTCPCPEER_H
#define OZZLTCPCPEER_H

#include "../common/ozzlprotocommon.h"

#include <QObject>
#include <QCommandLineParser>
#include <QString>
#include <QTcpSocket>
#include <QFile>
#include <QDataStream>

class OzzlTcpPeer : public QObject
{
    Q_OBJECT

public:
    explicit OzzlTcpPeer(QCommandLineParser* pParser, QObject *pP = nullptr);

public slots:
    void slotConnectToSrv();

private slots:
    bool slotRequest();
    void slotResponseHandler();
    void slotOnDisconnect();

private:
    bool getHeader();

    QCommandLineParser* mpParser;
    QTcpSocket* mpTcpSocket;
    Protocol::Request mRequest;
    Protocol::ResponseHeader mResponseHeader;
    bool mIsHeaderOk = false;
    qint64 mBodyBytesReceived = 0;
    QFile mFile;
    QDataStream mOut;
};

#endif // OZZLTCPCPEER_H