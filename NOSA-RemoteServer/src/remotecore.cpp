#include "remotecore.h"
#include <iostream>
#include "helper.h"
#include "baseconnection.h"
#include "connectioncontainer.h"

RemoteCore::RemoteCore(int _port) : port(_port)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA data;
    sockaddr_in sockname;

    if (WSAStartup(wVersionRequested, &data) != 0)
    {
        std::cerr << "E: Couldn't initialize sockets." << std::endl;
        initialized = false;
        return;
    }

    if ((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        std::cerr << "E: Couldn't create socket." << std::endl;
        initialized = false;
        return;
    }

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = INADDR_ANY;

    if (bind(mainSocket, (sockaddr *)&sockname, sizeof(sockname)) == SOCKET_ERROR)
    {
        std::cerr << "E: Couldn't bind socket." << std::endl;
        initialized = false;
        return;
    }

    if (listen(mainSocket, 10) == SOCKET_ERROR)
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

    int addrLen;
    sockaddr_in clientInfo;

    do
    {
        addrLen = sizeof(clientInfo);
        TCPSocket _socket = accept(mainSocket, (sockaddr*)&clientInfo, &addrLen);
        NetworkSocket* socket = new NetworkSocket(_socket);
        BaseConnection* connection = BaseConnection::estabilishConnection(socket);
        sConnections.insert(connection);

        _beginthreadex(NULL, 0, RemoteCore::handleConnection, connection, 0, NULL);

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
    closesocket(mainSocket);

    return 0;
}


unsigned __stdcall RemoteCore::handleConnection(void* data)
{
    BaseConnection* connection = static_cast<BaseConnection*>(data);
    connection->read();
    _endthread(); // is it necessery?
}


