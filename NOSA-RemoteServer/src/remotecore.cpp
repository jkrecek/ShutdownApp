#include "remotecore.h"
#include <iostream>
#include "helper.h"
#include "connectioncontainer.h"
#include <unistd.h>

#ifndef _WIN32
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/socket.h> // Needed for the socket functions
    #include <netdb.h>      // Needed for the socket functions
#endif

RemoteCore::RemoteCore(int _port)
    : port(_port), server(NULL)
{
    prepareSockets();

    server = ServerSocket::createSocket(port);
}

RemoteCore::~RemoteCore()
{

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

        /*std::cout << "User: " << connection->getUser() << std::endl;
        std::cout << "Pass: " << connection->getPass() << std::endl;
        std::cout << "type: " << connection->getType() << std::endl;*/

        /*while(1)
        {
            const char* theRealFirstLine = connection->getSocket()->readLine();
            std::cout << "INC MSG: " << theRealFirstLine << std::endl;
        }*/

        /*Connection con(this, socket, clientInfo);
        if (con.getSocket() == INVALID_SOCKET)
        {
            std::cerr << "E: Couldn't accept connection." << std::endl;
            break;
        }

        con.handle();*/
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
    pthread_create(NULL, NULL, RemoteCore::handleConnection, connection);
#endif
}

void RemoteCore::endThread()
{
    // must be called?
#ifdef _WIN32
    _endthread();
#else
    pthread_exit(NULL);
#endif
}


THREAD_RETURN_TYPE RemoteCore::handleConnection(void* data)
{
    BaseConnection* connection = static_cast<BaseConnection*>(data);
    connection->read();
    endThread();
}

void RemoteCore::prepareSockets()
{
#ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA data;

    if (WSAStartup(wVersionRequested, &data) != 0)
    {
        std::cerr << "E: Couldn't initialize sockets." << std::endl;
        initialized = false;
        return;
    }
#else
    return;
#endif
}

void RemoteCore::cleanSockets()
{
#ifdef _WIN32
    WSACleanup();
#else
    return;
#endif
}
