#include "core.h"
#include <iostream>
#include "helper.h"
#include "connection.h"

Core::Core(int _port) : port(_port), serverControl(NULL)
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

Core::~Core()
{

}

int Core::run()
{
    if (!initialized)
        return -1;

    int addrLen;
    sockaddr_in clientInfo;

    do
    {
        addrLen = sizeof(clientInfo);
        SOCKET socket = accept(mainSocket, (sockaddr*)&clientInfo, &addrLen);

        Connection con(this, socket, clientInfo);
        if (con.getSocket() == INVALID_SOCKET)
        {
            std::cerr << "E: Couldn't accept connection." << std::endl;
            break;
        }

        con.handle();
    } while (true);

    std::cout << "Ending" << std::endl;
    closesocket(mainSocket);

    return 0;
}


