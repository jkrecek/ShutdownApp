#include "mainsocket.h"
#include <iostream>
#include <ctime>


MainSocket::MainSocket(TCPSocket socket, sockaddr_in info)
    : NetworkSocket(socket, info)
{

}

MainSocket* MainSocket::createSocket(std::string hostname, int port)
{
    hostent *host;
    sockaddr_in sockInfo;
    TCPSocket baseSocket;

    if ((host = gethostbyname(hostname.c_str())) == NULL)
    {
        std::cout << "Wrong address" << std::endl;
        return NULL;
    }

    if ((baseSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
    {
        std::cout << "Could not create socket" << std::endl;
        return NULL;
    }


    sockInfo.sin_family = AF_INET;
    sockInfo.sin_port = htons(port);
    memcpy(&(sockInfo.sin_addr), host->h_addr, host->h_length);

    if (connect(baseSocket, (sockaddr *)&sockInfo, sizeof(sockInfo)) == SOCKET_ERROR)
    {
        std::cout << "Could not link connection" << std::endl;
        return NULL;
    }

    return new MainSocket(baseSocket, sockInfo);
}
