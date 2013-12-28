#include "pccore.h"
#include <iostream>
#include "helper.h"

PCCore::PCCore(int _port)
    : BaseCore(), port(_port), socket(NULL), handler(NULL)
{
    std::cout << "Connecting to server" << std::endl;
    if (!prepareSockets())
        return;

    socket = MainSocket::createSocket("localhost", port);
    if (socket)
    {
        std::cout << "Connected" << std::endl;
        handler = new PacketHandler(socket);
    }
}

PCCore::~PCCore()
{

}

int PCCore::run()
{
    if (!socket)
        return -1;

    do
    {
        std::string line = socket->readLine();
        handler->accepted(line);

    } while (true);

    std::cout << "Ending" << std::endl;
    socket->close();

    return 0;
}


