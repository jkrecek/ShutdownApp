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
#ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA data;

    if (WSAStartup(wVersionRequested, &data) != 0)
    {
        std::cerr << "E: Couldn't initialize sockets." << std::endl;
        initialized = false;
        return;
    }
#endif

    sockaddr_in sockname;
    TCPSocket _server;
    if ((_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        std::cerr << "E: Couldn't create socket." << std::endl;
        initialized = false;
        return;
    }



    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = INADDR_ANY;


    if (bind(_server, (sockaddr *)&sockname, sizeof(sockname)) == SOCKET_ERROR)
    {
        std::cerr << "E: Couldn't bind socket." << std::endl;
        initialized = false;
        return;
    }

    server = new NetworkSocket(_server, sockname);

    if (listen(server->getSocket(), 10) == SOCKET_ERROR)
    {
        std::cerr << "E: Couldn't create listining queue." << std::endl;
        initialized = false;
        return;
    }

    initialized = true;
}

RemoteCore::~RemoteCore()
{

}

int RemoteCore::run()
{
    if (!initialized)
        return -1;

    SocketLength addrLen;
    sockaddr_in clientInfo;

    do
    {
        addrLen = sizeof(clientInfo);
        TCPSocket _socket = accept(server->getSocket(), (sockaddr*)&clientInfo, &addrLen);
        std::cout << "IC[" << _socket << "]" << std::endl;
        NetworkSocket* socket = new NetworkSocket(_socket, clientInfo);
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
#ifdef _WIN32
        (mainSocket);
#else
    // TODO
#endif

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


THREAD_RETURN_TYPE RemoteCore::handleConnection(void* data)
{
    BaseConnection* connection = static_cast<BaseConnection*>(data);
    connection->read();
#ifdef _WIN32
    _endthread(); // is it necessery?
#else
    pthread_exit(NULL);
#endif
}


