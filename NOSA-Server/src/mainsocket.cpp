#include "mainsocket.h"
#include <iostream>
#include <ctime>
#include "defines.h"
#ifndef _WIN32
    #include <netdb.h>
    #include <stdio.h>
    #include <string.h>
#endif


MainSocket::MainSocket(TCPSocket socket, sockaddr_in info)
    : NetworkSocket(socket, info)
{

}

MainSocket* MainSocket::createSocket(Configuration* configuration)
{
    hostent *host;
    sockaddr_in sockInfo;
    TCPSocket baseSocket;

    const char* hostname = configuration->getString("REMOTE_ADDRESS").c_str();

    if ((host = gethostbyname(hostname)) == NULL)
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
    sockInfo.sin_port = htons(SOCKET_PORT);
    memcpy(&(sockInfo.sin_addr), host->h_addr, host->h_length);

    if (connect(baseSocket, (sockaddr *)&sockInfo, sizeof(sockInfo)) == SOCKET_ERROR)
    {
        std::cout << "Could not link connection" << std::endl;
        return NULL;
    }


    MainSocket* sock = new MainSocket(baseSocket, sockInfo);
    sock->sendMsg(generateAuthMessage(configuration));
    return sock;
}

const char* MainSocket::generateAuthMessage(Configuration *configuration)
{
    char buffer[1024];
    snprintf(buffer, 1024, "type=PC user=%s pass=%s", configuration->getString("USER").c_str(), configuration->getString("PASS").c_str());
    return strdup(buffer);
}
