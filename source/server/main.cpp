#include "ozzltcpserver.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    OzzlTcpServer server(&a);

    return a.exec();
}