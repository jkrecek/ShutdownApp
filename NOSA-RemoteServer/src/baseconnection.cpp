#include "baseconnection.h"
#include "androidconnection.h"
#include "pcconnection.h"
#include "helper.h"
#include "socketclosedexception.h"
#include "connectioncontainer.h"

#include <iostream>

BaseConnection::BaseConnection(NetworkSocket *_socket, ConnectionType _type)
    : socket(_socket), type(_type)
{
}

BaseConnection::~BaseConnection()
{
    sConnections.remove(this);
    delete socket;
}

BaseConnection* BaseConnection::estabilishConnection(NetworkSocket *socket)
{
    Packet* packet = socket->readPacket();

    NVMap parameters = socket->parsePacket(packet->getMessage());
    if (!parameters.contains("type") ||
        !parameters.contains("user") ||
        !parameters.contains("pass"))
    {
        std::cout << "Invalid parameters supplied:" << std::endl;
        for (NVMap::iterator itr = parameters.begin(); itr != parameters.end(); ++itr)
            std::cout << "-- Name: " << itr->first << ", Value: " << itr->second << std::endl;
        return NULL;
    }

    BaseConnection* connection = NULL;
    if (Helper::iequals(parameters.get("type"), "ANDROID"))
        connection = new AndroidConnection(socket);
    else if (Helper::iequals(parameters.get("type"), "PC"))
        connection = new PCConnection(socket);
    else
        return NULL;

    connection->user = parameters.get("user");
    connection->pass = parameters.get("pass");

    delete packet;
    return connection;

}

void BaseConnection::read()
{
    Packet* packet;
    try
    {
        while(socket && socket->isOpen())
        {
            packet = socket->readPacket();

            if (!packet)
                continue;

            if (packet->getMessage() == "CLOSE")
                break;

            redistributePacket(packet);
            delete packet;
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }

    delete packet;
}
