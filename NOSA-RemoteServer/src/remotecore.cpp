#include "remotecore.h"
#include <iostream>
#include "helper.h"
#include "connectioncontainer.h"
#include <unistd.h>
#include "socketclosedexception.h"

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
        delete itr.get();

    delete server;
}

int RemoteCore::run()
{
    BaseConnection* connection = NULL;
    while (server && server->isOpen())
    {
        NetworkSocket* socket = server->acceptConnection();
        try
        {
            connection = BaseConnection::estabilishConnection(socket);
        }
        catch (SocketClosedException& /*e*/)
        {
            std::cout << "Exception occured in estabilishing connection. Socket is beeing closed." << std::endl;
            socket->close();
            continue;
        }

        sConnections.insert(connection);

        startThread(connection);
    }

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
    delete connection;
    endThread();
}

