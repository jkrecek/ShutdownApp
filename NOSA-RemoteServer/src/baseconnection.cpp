#include "baseconnection.h"
#include "androidconnection.h"
#include "pcconnection.h"
#include "helper.h"

#include <iostream>

BaseConnection::BaseConnection(NetworkSocket *_socket, ConnectionType _type)
    : socket(_socket), type(_type)
{
}

BaseConnection::~BaseConnection()
{
    delete socket;
}

BaseConnection* BaseConnection::estabilishConnection(NetworkSocket *socket)
{
    std::string line = socket->readLine();

    NVMap parameters = socket->parsePacket(line);
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
    return connection;

}

void BaseConnection::read()
{
    while(socket)
    {
        std::string line = socket->readLine();
        if (line.empty())
            continue;

        if (line == "CLOSE")
        {
            delete socket;
            socket = NULL;
            continue;
        }

        redistributeLine(line);
    }
}
