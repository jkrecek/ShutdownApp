#include "baseconnection.h"
#include "androidconnection.h"
#include "pcconnection.h"
#include "helper.h"
#include "socketclosedexception.h"
#include "connectioncontainer.h"

#include <iostream>

BaseConnection::BaseConnection(NetworkSocket *_socket, ConnectionType _type, std::string _user, std::string _pass)
    : socket(_socket), type(_type), user(_user), pass(_pass)
{
}

BaseConnection::~BaseConnection()
{
    sConnections.remove(this);
    delete socket;
}

BaseConnection* BaseConnection::estabilishConnection(SocketPacket* soPa)
{
    NVMap parameters = NetworkSocket::parsePacket(soPa->getPacket()->getMessage());
    if (!parameters.contains("type") ||
        !parameters.contains("user") ||
        !parameters.contains("pass"))
    {
        std::cout << "Invalid login message, terminating connection." << std::endl;
        return NULL;
    }

    std::string user = parameters.get("user");
    std::string pass = parameters.get("pass");
    if (Helper::iequals(parameters.get("type"), "ANDROID"))
        return new AndroidConnection(soPa->getSocket(), user, pass);
    else if (Helper::iequals(parameters.get("type"), "PC")) {
        bool exists = sConnections.getPCConnection(user, pass);
        if (exists)
        {
            std::cout << "Connection already exists" << std::endl;
            soPa->respond("ALREADY_EXISTS", false);
            return NULL;
        } else
            return new PCConnection(soPa->getSocket(), user, pass);
    }
    else
        return NULL;
}

void BaseConnection::read()
{
    try
    {
        while(socket && socket->isOpen())
        {
            Packet packet = socket->readPacket();

            /*if (packet->getMessage() == "CLOSE")
                break;*/

            if (!handlePacket(&packet))
                redistributePacket(&packet);
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }
}
