#ifndef BASECONNECTION_H
#define BASECONNECTION_H

#include "networksocket.h"
#include "packet.h"

#include "socketpacket.h"

enum ConnectionType
{
    TYPE_ANDROID = 0,
    TYPE_PC,
    TYPE_COUNT
};

class BaseConnection
{
public:
    ~BaseConnection();

    static BaseConnection* estabilishConnection(SocketPacket* soPa);

    ConnectionType getType() const { return type; }
    std::string getUser() const { return user; }
    std::string getPass() const { return pass; }
    NetworkSocket* getSocket() const { return socket; }

    void read();

    virtual void redistributePacket(Packet* packet) = 0;
    virtual bool handlePacket(Packet* packet) = 0;

protected:
    BaseConnection(NetworkSocket* _socket, ConnectionType _type, std::string _user, std::string _pass);

    NetworkSocket* socket;
    ConnectionType type;
    std::string user;
    std::string pass;
};

#endif // BASECONNECTION_H
