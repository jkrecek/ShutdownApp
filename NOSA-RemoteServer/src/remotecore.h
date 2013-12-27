#ifndef CORE_H
#define CORE_H

#include <string>
#include <windows.h>
#include "ipaddress.h"
#include "networksocket.h"

class RemoteCore
{
public:
    RemoteCore(int port);
    ~RemoteCore();

    int run();

    static unsigned __stdcall handleConnection(void* data);

private:
    int port;
    bool initialized;
    TCPSocket mainSocket;
};

#endif // CORE_H
