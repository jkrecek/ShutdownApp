#ifndef CORE_H
#define CORE_H

#include <string>
#include "ipaddress.h"
#include "networksocket.h"
#include "baseconnection.h"
#include "serversocket.h"

#ifdef _WIN32
    #define THREAD_RETURN_TYPE unsigned __stdcall
    typedef int SocketLength;
#else
    #define THREAD_RETURN_TYPE void*
    typedef socklen_t SocketLength;
#endif


class RemoteCore
{
public:
    RemoteCore(int port);
    ~RemoteCore();

    int run();

    static THREAD_RETURN_TYPE handleConnection(void* data);

    static void startThread(BaseConnection* connection);
    static void endThread();

    static void prepareSockets();
    static void cleanSockets();

private:
    int port;
    ServerSocket* server;
};

#endif // CORE_H
