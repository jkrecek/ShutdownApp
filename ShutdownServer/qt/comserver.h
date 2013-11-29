#ifndef APPSERVICE_H
#define APPSERVICE_H

#include <QtNetwork/QTcpServer>
#include <QString>

class ComServer : public QTcpServer
{
    Q_OBJECT
 public:
     ComServer(quint16 port, QObject* parent = 0)
         : QTcpServer(parent), disabled(false)
     {
         listen(QHostAddress::Any, port);
     }

     void incomingConnection(int socket);

     void pause();

     void resume();

     QString handleIncomingMessage(QString message);

 private slots:
     void readClient();

     void discardClient();

 private:
     bool disabled;

};

#endif // APPSERVICE_H
