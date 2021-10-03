#include "ozzltcpserver.h"
#include "ozzltcpworker.h"

#include <unistd.h>
#include <sys/socket.h>

#include <QProcessEnvironment>
#include <QString>
#include <QHostAddress>
#include <QCoreApplication>

#include <QDebug>

int OzzlTcpServer::sighupFd[2] = {};
int OzzlTcpServer::sigtermFd[2] = {};

OzzlTcpServer::OzzlTcpServer(QObject* pP) : QTcpServer(pP)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd)) {
        QTextStream(stderr) << "Couldn't create HUP socketpair" << endl;
        QCoreApplication::exit(1);
    }
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd)) {
        QTextStream(stderr) << "Couldn't create TERM socketpair" << endl;
        QCoreApplication::exit(1);
    }

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, &QSocketNotifier::activated,
        this, &OzzlTcpServer::handleSigHup);
    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, &QSocketNotifier::activated,
        this, &OzzlTcpServer::handleSigTerm);

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
    if ( mOsSigReceived ) return;

    QTextStream(stdout) << "descriptor " << socketDescriptor
        << " for new incoming connection has been allocated" << endl;

    QThread* pSocThd = new QThread(this);
    OzzlTcpWorker* pCon = new OzzlTcpWorker(socketDescriptor);
    pCon->moveToThread(pSocThd);
    connect(pSocThd, &QThread::started, pCon, &OzzlTcpWorker::slotInit);
    connect(pSocThd, &QThread::finished, this, &OzzlTcpServer::slotRemoveThread);
    pSocThd->start();
    mIsThreadRunning[pSocThd] = true;
}

void OzzlTcpServer::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}

void OzzlTcpServer::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}

void OzzlTcpServer::handleSigTerm()
{
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    onOsSigExit();

    snTerm->setEnabled(true);
}

void OzzlTcpServer::handleSigHup()
{
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    onOsSigExit();

    snHup->setEnabled(true);
}

void OzzlTcpServer::onOsSigExit()
{
    mOsSigReceived = true;
    while ( !mIsThreadRunning.empty() ) {
        QCoreApplication::processEvents();
    }

    QCoreApplication::exit();
}

void OzzlTcpServer::slotRemoveThread()
{
    mIsThreadRunning.remove((QThread*)sender());
}