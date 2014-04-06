#ifndef BASECORE_H
#define BASECORE_H

enum State {
    STATE_DISCONNECTED = 0,
    STATE_INVALID_CONFIG,
    STATE_CONNECTED,
    STATE_SUCCESS
};

class BaseCore
{
public:
    BaseCore();

    static void endThread();

    static bool prepareSockets();
    static void cleanSockets();
};

#endif // BASECORE_H
