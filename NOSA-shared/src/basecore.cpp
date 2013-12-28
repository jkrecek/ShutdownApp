#include "basecore.h"
#include <unistd.h>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
    #include <sys/socket.h> // Needed for the socket functions
    #include <netdb.h>      // Needed for the socket functions
#endif

BaseCore::BaseCore()
{
}

void BaseCore::endThread()
{
    // must be called?
#ifdef _WIN32
    _endthread();
#else
    pthread_exit(NULL);
#endif
}


bool BaseCore::prepareSockets()
{
#ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA data;

    if (WSAStartup(wVersionRequested, &data) != 0)
    {
        std::cout << "E: Couldn't initialize sockets." << std::endl;
        return false;
    }
    return true;
#else
    return true;
#endif
}

void BaseCore::cleanSockets()
{
#ifdef _WIN32
    WSACleanup();
#else
    return;
#endif
}
