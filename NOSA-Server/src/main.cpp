#include "pccore.h"
#include <iostream>

int main()
{
    PCCore core(SOCKET_PORT);
    int result = core.run();
    core.cleanSockets();
    return result;
}


