#ifndef CORE_H
#define CORE_H

#include <string>
#include "ipaddress.h"
#include "servercontrol.h"
#include "basecore.h"
#include "mainsocket.h"
#include "packethandler.h"

class PCCore : public BaseCore
{
public:
    PCCore(int port);
    ~PCCore();

    int run();

private:
    int port;
    bool initialized;
    MainSocket* socket;
    PacketHandler* handler;
};

#endif // CORE_H
