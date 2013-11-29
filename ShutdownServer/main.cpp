#include <QCoreApplication>
#include <QProcess>
#include "comserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ComServer server(3691);

    return a.exec();
}
