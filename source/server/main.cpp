#include "ozzltcpserver.h"

#include <signal.h>

#include <QCoreApplication>

static int setup_unix_signal_handlers()
{
    struct sigaction hup, term;

    hup.sa_handler = OzzlTcpServer::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0))
       return 1;

    term.sa_handler = OzzlTcpServer::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0))
       return 2;

    return 0;
}

int main(int argc, char *argv[])
{
    setup_unix_signal_handlers();

    QCoreApplication a(argc, argv);

    OzzlTcpServer server(&a);

    return a.exec();
}