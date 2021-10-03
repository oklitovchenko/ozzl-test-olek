#include "ozzltcppeer.h"

#include <QCoreApplication>
#include <QCommandLineOption>
#include <QThread>
#include <QObject>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({
        {"c","config-file"},
        "Client configuration <file>.", "config file",
        "/etc/ozzl/test-olek-client/client.conf"});
    parser.addOption({
        {"r", "request-value"},
        "Client request <value>.", "request value",
        ""});
    parser.addOption({
        {"v", "protocol-version"},
        "Client protocol <version> for TCP interaction.", "protocol version",
        ""});
    parser.addOption({
        {"o", "output-file"},
        "Client output <file>.", "output file",
        ""});
    parser.process(QCoreApplication::arguments());

    OzzlTcpPeer p(&parser);

    QThread t(&a);
    p.moveToThread(&t);
    QObject::connect(&t, &QThread::started, &p, &OzzlTcpPeer::slotConnectToSrv);
    t.start();

    int e = a.exec();
    if (t.isRunning()) t.wait();

    return e;
}