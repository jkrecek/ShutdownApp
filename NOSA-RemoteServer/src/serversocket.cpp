#include "serversocket.h"
#include <iostream>

ServerSocket::ServerSocket(TCPSocket socket, sockaddr_in info)
    : NetworkSocket(socket, info)
{

}

ServerSocket* ServerSocket::createSocket(int port)
{
    sockaddr_in sockname;
    TCPSocket server;
    if ((server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        std::cerr << "E: Couldn't create socket." << std::endl;
        return NULL;
    }

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = INADDR_ANY;


    if (bind(server, (sockaddr *)&sockname, sizeof(sockname)) == SOCKET_ERROR)
    {
        std::cerr << "E: Couldn't bind socket." << std::endl;
        return NULL;
    }

    if (listen(server, 10) == SOCKET_ERROR)
    {
        std::cerr << "E: Couldn't create listining queue." << std::endl;
        return NULL;
    }

    return new ServerSocket(server, sockname);
}

NetworkSocket* ServerSocket::acceptConnection()
{
    sockaddr_in clientInfo;
    SocketLength addrLen = sizeof(clientInfo);
    TCPSocket clientSocket = accept(socket, (sockaddr*)&clientInfo, &addrLen);
    return new NetworkSocket(clientSocket, clientInfo);
}

void ServerSocket::close()
{
    doClose();
}
