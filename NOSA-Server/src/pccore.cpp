#include "pccore.h"
#include <iostream>
#include "helper.h"
#include "socketclosedexception.h"

PCCore::PCCore(int _port)
    : BaseCore(), port(_port), socket(NULL), handler(NULL)
{
    std::cout << "Connecting to server" << std::endl;
    if (!prepareSockets())
        return;

    socket = MainSocket::createSocket("54.194.29.200", port);
    if (socket)
    {
        std::cout << "Connected" << std::endl;
        handler = new PacketHandler(socket);
    }
}

PCCore::~PCCore()
{
    delete socket;
}

int PCCore::run()
{
    try
    {
        while(socket->isOpen())
        {
            std::string line = socket->readLine();
            handler->accepted(line);
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }

    return 0;
}


