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

BaseConnection* BaseConnection::estabilishConnection(NetworkSocket *socket)
{
    Packet packet = socket->readPacket();

    NVMap parameters = socket->parsePacket(packet.getMessage());
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
        return new AndroidConnection(socket, user, pass);
    else if (Helper::iequals(parameters.get("type"), "PC")) {
        PCConnection * existing = sConnections.getPCConnection(user, pass);
        if (!existing)
            return new PCConnection(socket, user, pass);
        else
        {
            std::cout << "Connection already exists" << std::endl;
            socket->sendResponse(&packet, "ALREADY_EXISTS");
            return NULL;
        }
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
