#ifndef CONNECTION_H
#define CONNECTION_H

#include <windows.h>
#include "core.h"

class Connection
{
public:
    Connection(Core* core, SOCKET socket, sockaddr_in info);
    SOCKET getSocket() {
        return socket;
    }

    void handle();
    void process(const char* message);
    void respond(const char* message);
    void responseClose(const char* message);
    void close();

private:
    const char* safeResponseFromat(const char* message);
    Core* core;
    SOCKET socket;
    sockaddr_in info;
};

#endif // CONNECTION_H
