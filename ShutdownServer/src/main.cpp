#include "core.h"
#include <iostream>

#define PORT 3691

int main()
{
    Core core(PORT);
    int result = core.run();
    WSACleanup();
    return result;
}


