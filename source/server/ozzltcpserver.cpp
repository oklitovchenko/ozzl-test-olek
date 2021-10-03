#include "ozzltcpserver.h"
#include "ozzltcpworker.h"

#include <QProcessEnvironment>
#include <QString>
#include <QHostAddress>
#include <QCoreApplication>
#include <QThread>

#include <QDebug>

OzzlTcpServer::OzzlTcpServer(QObject* pP) : QTcpServer(pP)
{
    QProcessEnvironment envs = QProcessEnvironment::systemEnvironment();
    QString addr = envs.value("OZZL_TEST_SRV_BINDADDR", "0.0.0.0");
    QString prt = envs.value("OZZL_TEST_SRV_BINDPRT", "6006");

    if ( !listen(QHostAddress(addr), prt.toInt()) ) {
        QTextStream(stderr) << "ozzl test: server: is not started" << endl;
        QCoreApplication::exit(1);
    }

    QTextStream(stdout) << "ozzl test: server: is started on "
        << addr << ":" << prt << endl;
}

void OzzlTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTextStream(stdout) << "descriptor " << socketDescriptor
        << " for new incoming connection has been allocated" << endl;

    QThread* pSocThd = new QThread(this);
    OzzlTcpWorker* pCon = new OzzlTcpWorker(socketDescriptor);
    pCon->moveToThread(pSocThd);
    connect(pSocThd, &QThread::started, pCon, &OzzlTcpWorker::slotInit);
    pSocThd->start();
}
