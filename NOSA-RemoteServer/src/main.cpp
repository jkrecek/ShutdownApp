#include "configuration.h"
#include "remotecore.h"
#include <iostream>
#include <unistd.h>

int main()
{
    Configuration::loadFile("D:\\Dropbox\\Dev\\ShutdownApp\\config.conf");
    int result = 0;
    sleep(15);
    while(true)
    {
        RemoteCore core(SOCKET_PORT);
        bool success = core.socketCreated();
        if (success)
        {
            result = core.run();
            core.cleanSockets();
        } else
            sleep(15);
    }
    return result;
}

