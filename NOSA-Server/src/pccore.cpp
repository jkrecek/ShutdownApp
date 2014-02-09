#include "pccore.h"
#include <iostream>
#include "helper.h"
#include "socketclosedexception.h"
#include "defines.h"

PCCore::PCCore()
    : BaseCore(), socket(NULL)
{
    std::cout << "Loading config ..." << std::endl;
    configuration = Configuration::loadFile(CONFIG_FILENAME);
    if (!configuration->fileExists())
    {
        std::cout << "Config file not found!" << std::endl;
        exit(0);
    }
    else if (const char* error = configuration->isValid())
    {
        std::cout << "Configuration file is not valid! (" << error << ")" << std::endl;
        exit(0);
    }
    else
        std::cout << "Config loaded!" << std::endl;

    std::cout << "Connecting to server ..." << std::endl;
    if (!prepareSockets())
        return;

    socket = MainSocket::createSocket(configuration);

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
            Packet packet = socket->readPacket();
            SocketPacket sopa(socket, &packet);
            handler.handle(&sopa);
        }
    }
    catch (SocketClosedException& /*e*/) { /* just interrupt */ }


    return 0;
}


