#include "ozzltcpworker.h"

#include <QCoreApplication>
#include <QHostAddress>
#include <QString>
#include <QThread>

OzzlTcpWorker::OzzlTcpWorker(qintptr socketDescriptor, QObject* pP)
    : QObject(pP)
{
    mSocketDescriptor = socketDescriptor;
}

void OzzlTcpWorker::slotInit()
{
    mpTcpSocket = new QTcpSocket(this);
    if ( !mpTcpSocket->setSocketDescriptor(mSocketDescriptor) ) {
        QTextStream(stderr)
            << "ozzl test task: server: could not open socket" << endl;
        deleteLater();
        thread()->quit();
        return;
    }

    connect(mpTcpSocket, &QTcpSocket::readyRead,
        this, &OzzlTcpWorker::slotReqHandler);
    connect(mpTcpSocket, &QTcpSocket::disconnected,
        this, &OzzlTcpWorker::slotOnDisconnect);
}

void OzzlTcpWorker::slotReqHandler()
{
    Protocol::Request request =
        *(Protocol::Request*)mpTcpSocket->read(sizeof(Protocol::Request)).data();

    /*
     *  UNEXPECTED PROTOCOL ERROR
     */
    if ( (request.header.protocolVersion & 0xFFFF0000) != 0xAAFF0000 ) {
        mpTcpSocket->close();
        return;
    }

    Protocol::ResponseHeader resp;
    double resVal = ++request.request;

    switch (request.header.protocolVersion)
    {
        #define __1M 1000000
        #define __1_5M 1500000

        case Protocol::VER_01:
        /*
         *  VER_01 -> 1 000 000 double in response body
         *  To be implemented as a separate class for protocol VER_01
         */
            resp.protocolVersion = Protocol::VER_01;
            resp.bodySize = __1M * sizeof(double);
            sendResponse(&resp, &resVal, __1M); // check result to be implemented
            break;

        case Protocol::VER_02:
        /*
         *  VER_01 -> 1 500 000 double in response body
         *  To be implemented as a separate class for protocol VER_02
         */
            resp.protocolVersion = Protocol::VER_02;
            resp.bodySize = __1_5M * sizeof(double);
            sendResponse(&resp, &resVal, __1_5M); // check result to be implemented
            break;

        default:
        /*
         *  PROTOCOL VERSION MISMATCH
         */
            resp.protocolVersion = Protocol::VER_UNDEFINED;
            resp.error = Protocol::ERR_PROTO_VER_MISMATCH;
            QByteArray e("Protocol version mismatch. Supported versions:");
            for ( const auto v : Protocol::AllVers )
                e.append(" [0x" + QString().setNum(v, 16) + "]");
            resp.bodySize = e.size();
            writeToSocket(QByteArray::fromRawData((char*)&resp,
                sizeof(Protocol::ResponseHeader)) + e); // check result to be implemented
    }
}

bool OzzlTcpWorker::writeToSocket(QByteArray d)
{
    if ( mpTcpSocket->state() == QAbstractSocket::ConnectedState ) {
        mpTcpSocket->write(d);
        return mpTcpSocket->waitForBytesWritten();
    } else return false;
}

void OzzlTcpWorker::slotOnDisconnect()
{
    deleteLater();
    thread()->quit();
}

bool OzzlTcpWorker::sendResponse(Protocol::ResponseHeader* pH, double* pVal, quint64 amount)
{
    if ( !writeToSocket(QByteArray::fromRawData((char*)pH, sizeof(Protocol::ResponseHeader))) )
        return false;
    for ( quint64 i = 0; i < amount; i++ ) {
        if ( !writeToSocket(QByteArray::fromRawData((char*)pVal, sizeof(double))) )
            return false;
    }
    return true;
}