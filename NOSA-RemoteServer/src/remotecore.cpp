#include "remotecore.h"
#include <iostream>
#include "helper.h"
#include "connectioncontainer.h"
#include <unistd.h>

#ifndef _WIN32
    #include <pthread.h>
    #include <sys/socket.h> // Needed for the socket functions
    #include <netdb.h>      // Needed for the socket functions
#endif

RemoteCore::RemoteCore(int port)
    : BaseCore(), server(NULL)
{
    if (!prepareSockets())
        return;

    server = ServerSocket::createSocket(port);
}

RemoteCore::~RemoteCore()
{
    for(ConnectionContainer::iterator itr = sConnections.begin(); itr.hasMore(); itr.next())
        itr.get()->getSocket()->close();

    if (server)
        server->close();
}

int RemoteCore::run()
{
    if (!server)
        return -1;

    do
    {
        NetworkSocket* socket = server->acceptConnection();
        BaseConnection* connection = BaseConnection::estabilishConnection(socket);
        sConnections.insert(connection);

        startThread(connection);
    } while (true);

    std::cout << "Ending" << std::endl;
    server->close();

    return 0;
}

void RemoteCore::startThread(BaseConnection *connection)
{
#ifdef _WIN32
    _beginthreadex(NULL, 0, RemoteCore::handleConnection, connection, 0, NULL);
#else
    pthread_t t;
    pthread_create(&t, NULL, RemoteCore::handleConnection, connection);
#endif
}

THREAD_RETURN_TYPE RemoteCore::handleConnection(void* data)
{
    BaseConnection* connection = static_cast<BaseConnection*>(data);
    connection->read();
    endThread();
}

