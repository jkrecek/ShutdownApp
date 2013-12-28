#ifndef BASECORE_H
#define BASECORE_H

#define SOCKET_PORT 3691

class BaseCore
{
public:
    BaseCore();

    static void endThread();

    static bool prepareSockets();
    static void cleanSockets();
};

#endif // BASECORE_H
