#include "ozzltcppeer.h"

#include <QSettings>
#include <QString>
#include <QHostAddress>
#include <QTimer>
#include <QCoreApplication>
#include <QThread>

OzzlTcpPeer::OzzlTcpPeer(QCommandLineParser* pParser, QObject* pP) : QObject (pP)
{
    mpParser = pParser;
}

void OzzlTcpPeer::slotConnectToSrv()
{
    QString cnfFile = mpParser->value("config-file");
    QSettings settings(cnfFile, QSettings::NativeFormat, this);

    QString addr =
        settings.value("global/OZZL_TEST_SRV_IP", "127.0.0.1").toString();
    QString prt =
        settings.value("global/OZZL_TEST_SRV_PORT", "6006").toString();
    QString timeout =
        settings.value("global/OZZL_TEST_CONNTIMEOUT", "1000").toString();
    QString req =
        settings.value("request/OZZL_TEST_REQUEST", "1.5").toString();
    QString protoVer =
        settings.value("request/OZZL_TEST_PROTOCOLVERSION", "AAFF0001").toString();
    QString fileName =
        settings.value("rezult/OZZL_TEST_FILE", "out-file.double").toString();

    bool ok;
    unsigned int hex = protoVer.toUInt(&ok, 16);
    mRequest.header.protocolVersion = (Protocol::Versions)(ok ? hex : 0xAAFF0001);
    mFile.setFileName(fileName);
    mRequest.request = req.toDouble();

    mpTcpSocket = new QTcpSocket(this);
    connect(mpTcpSocket, &QTcpSocket::readyRead,
        this, &OzzlTcpPeer::slotResponseHandler);
    connect(mpTcpSocket, &QTcpSocket::disconnected,
        this, &OzzlTcpPeer::slotOnDisconnect);

    // If connection to server is ok, send request to server after 3000ms delay
    mpTcpSocket->connectToHost(QHostAddress(addr), prt.toInt());
    if ( mpTcpSocket->waitForConnected(timeout.toInt()) )
        QTimer::singleShot(3000, this, &OzzlTcpPeer::slotRequest);
    else {
        QTextStream(stderr)
            << "ozzl test: client: could not connect to server" << endl;
        QCoreApplication::exit(1);
        thread()->quit();
    }
}

void OzzlTcpPeer::slotResponseHandler() // Handler to process response from server
{
    if ( !mIsHeaderOk ) {
        if ( !getHeader() ) // If it first data, get header of response
            return;
        if ( !mFile.open(QFile::WriteOnly) ) { // Open output file if header is ok
            QTextStream(stderr)
                << "ozzl test: client: could not open file for writing" << endl;
            QCoreApplication::exit(1);
            thread()->quit();
            return;
        }
        mOut.setDevice(&mFile); // Prepare to put response body to file
        return;
    }

    // If header is ok and protocols are matched continue read chunk of body data
    QByteArray b = mpTcpSocket->readAll();
    mBodyBytesReceived += b.size();

    // put data into the file separately as each double value
    for ( int i = 0; i < b.size() / sizeof(double); i++ )
        mOut << ((double*)b.data())[i];

    // Quit after all data done
    if ( mBodyBytesReceived >= mResponseHeader.bodySize ) {
        QTextStream(stdout) << "ozzl test: client: received total bytes: "
            << mBodyBytesReceived << endl;

        mFile.close();
        mpTcpSocket->close();
    }
}

void OzzlTcpPeer::slotOnDisconnect()
{
    if ( !mResponseHeader.bodySize )
        QTextStream(stderr)
            << "ozzl test: client: unexpected protocol error" << endl;;
    QCoreApplication::exit(0); // Just for the test task
    thread()->quit();
}

bool OzzlTcpPeer::slotRequest()
{
    // Slot to send request to server after 3000ms timeout by QTimer signal
    if ( mpTcpSocket->state() == QAbstractSocket::ConnectedState ) {
        mpTcpSocket->write(
            QByteArray::fromRawData((char*)&mRequest, sizeof(Protocol::Request)));
        return mpTcpSocket->waitForBytesWritten();
    } else return false;
}

bool OzzlTcpPeer::getHeader()
{
    int s = sizeof(Protocol::ResponseHeader);
    if (
        mpTcpSocket->bytesAvailable() >= s &&
        (mResponseHeader =
            *(Protocol::ResponseHeader*)(mpTcpSocket->read(s)).data()).bodySize &&
    // Check protocol marker 0xAAFF. If no 0xAAFF at the begin of data from server
    // notify about unexpected protocol error
        (mResponseHeader.protocolVersion & 0xFFFF0000) == 0xAAFF0000
    )
    {
        // Check if protocol version in response are matched with request and
        // defined. If no notify about version mismatch protocol error
        if ( mResponseHeader.protocolVersion == Protocol::VER_UNDEFINED ||
            mRequest.header.protocolVersion !=  mResponseHeader.protocolVersion )
        {
            QTextStream(stderr)
                << "ozzl test: client: Error: 0x" +
                    QString().setNum(mResponseHeader.error, 16)
                << " " << mpTcpSocket->read(mResponseHeader.bodySize) << endl;
            if ( mpTcpSocket->state() == QAbstractSocket::ConnectedState )
                mpTcpSocket->close();
            QCoreApplication::exit(1);
            return false;
        }
        return mIsHeaderOk = true;
    } else {
        QTextStream(stderr)
            << "ozzl test: client: unexpected protocol error" << endl;
        if ( mpTcpSocket->state() == QAbstractSocket::ConnectedState )
            mpTcpSocket->close();
        QCoreApplication::exit(1);
        return false;
    }
}