#ifndef BASECONNECTION_H
#define BASECONNECTION_H

#include "networksocket.h"

enum ConnectionType
{
    TYPE_ANDROID,
    TYPE_PC
};

class BaseConnection
{
public:
    ~BaseConnection();

    static BaseConnection* estabilishConnection(NetworkSocket* _socket);

    ConnectionType getType() const { return type; }
    std::string getUser() const { return user; }
    std::string getPass() const { return pass; }
    NetworkSocket* getSocket() const { return socket; }

    void read();

    virtual void redistributeLine(std::string line) = 0;

protected:
    BaseConnection(NetworkSocket* _socket, ConnectionType type);

    NetworkSocket* socket;
    ConnectionType type;
    std::string user;
    std::string pass;
};

#endif // BASECONNECTION_H
