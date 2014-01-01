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
    Packet* packet;
    try
    {
        while(socket->isOpen())
        {
            packet = socket->readPacket();
            handler->accepted(packet);
            delete packet;
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }

    delete packet;

    return 0;
}


