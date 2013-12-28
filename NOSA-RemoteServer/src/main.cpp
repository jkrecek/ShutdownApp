#include "configuration.h"
#include "remotecore.h"
#include <iostream>

int main()
{
    Configuration::loadFile("D:\\Dropbox\\Dev\\ShutdownApp\\config.conf");
    RemoteCore core(SOCKET_PORT);
    int result = core.run();
    core.cleanSockets();
    return result;
}

