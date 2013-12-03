#ifndef CORE_H
#define CORE_H

#include <string>
#include <windows.h>
#include "ipaddress.h"
#include "servercontrol.h"

class Core
{
public:
    Core(int port);
    ~Core();

    int run();

    ServerControl* getServerControl()
    {
        if (!serverControl)
            serverControl = new ServerControl();

        return serverControl;
    }

private:
    int port;
    bool initialized;
    SOCKET mainSocket;
    ServerControl* serverControl;
};

#endif // CORE_H
