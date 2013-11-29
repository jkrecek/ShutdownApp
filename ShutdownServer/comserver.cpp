#include "comserver.h"
#include <QtNetwork/QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QDebug>

#ifdef Q_OS_WIN
    const QString COMMAND_TURN_OFF = "shutdown /s /f /t 0";
    const QString COMMAND_RESTART = "shutdown /r /f /t 0";
    const QString COMMAND_LOCK = "rundll32.exe user32.dll,LockWorkStation";
    const QString COMMAND_SLEEP = "rundll32 powrprof.dll,SetSuspendState Standby";
#elif Q_OS_UNIX
    const QString COMMAND_TURN_OFF = "shutdown -h";
#endif

void ComServer::incomingConnection(int socket)
{
    if (disabled)
        return;

    QTcpSocket* s = new QTcpSocket(this);
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
}

void ComServer::pause()
{
    disabled = true;
}

void ComServer::resume()
{
    disabled = false;
}

void ComServer::readClient()
{
    if (disabled)
        return;

    QTcpSocket* socket = (QTcpSocket*)sender();
    QStringList lines;
    while(socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("\\r?\\n"));
        foreach (QString string, tokens)
            if (!string.trimmed().isEmpty())
                lines.push_back(string.trimmed());
    }

    foreach (QString message, lines) {
        QString response = handleIncomingMessage(message);
        if (!response.isEmpty()) {
            socket->write(response.toUtf8() + "\n");
            socket->flush();
        }
    }


    socket->close();
    socket->deleteLater();
}

QString ComServer::handleIncomingMessage(QString message) {

    if (message == "TURN_OFF") {
        system(COMMAND_TURN_OFF.toUtf8());
        return "OK";
    } else if (message == "RESTART") {
        system(COMMAND_RESTART.toUtf8());
        return "OK";
    } else if (message == "LOCK") {
        system(COMMAND_LOCK.toUtf8());
        return "OK";
    } else if (message == "SLEEP") {
        system(COMMAND_SLEEP.toUtf8());
        return "OK";
    }

    return NULL;
}

void ComServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}

