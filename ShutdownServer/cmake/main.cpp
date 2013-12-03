#include "core.h"

#define PORT 3691

int main()
{
    Core core(PORT);
    int result = core.run();
    WSACleanup();
    return result;
}


