#include "pccore.h"
#include <iostream>
#include "helper.h"
#include "socketclosedexception.h"
#include "defines.h"

PCCore::PCCore()
    : BaseCore(), socket(NULL)
{
    m_state = initialize();
}

State PCCore::initialize() {
    std::cout << "Loading config ..." << std::endl;
    configuration = Configuration::loadFile(CONFIG_FILENAME);
    if (!configuration->fileExists())
    {
        std::cout << "Config file not found!" << std::endl;
        return STATE_INVALID_CONFIG;
    }
    else if (const char* error = configuration->isValid())
    {
        std::cout << "Configuration file is not valid! (" << error << ")" << std::endl;
        return STATE_INVALID_CONFIG;
    }
    else {
        std::cout << "Config loaded!" << std::endl;

        std::cout << "Connecting to server ..." << std::endl;
        if (prepareSockets()) {
            socket = MainSocket::createSocket(configuration);
            return STATE_CONNECTED;
        }
    }
}

PCCore::~PCCore()
{
    delete socket;
}

int PCCore::run()
{
    if (!socket && m_state != STATE_SUCCESS)
        return 0;

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


