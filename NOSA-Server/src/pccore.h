#ifndef CORE_H
#define CORE_H

#include <string>
#include "ipaddress.h"
#include "servercontrol.h"
#include "basecore.h"
#include "mainsocket.h"
#include "packethandler.h"
#include "configuration.h"

class PCCore : public BaseCore
{
public:
    PCCore();
    ~PCCore();

    int run();

    bool socketCreated() { return socket; }

private:
    bool initialized;
    MainSocket* socket;
    PacketHandler* handler;
    Configuration* configuration;
};

#endif // CORE_H
