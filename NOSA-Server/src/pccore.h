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

    State initialize();


    State getState() const { return m_state; }

private:
    MainSocket* socket;
    PacketHandler handler;
    Configuration* configuration;

    State m_state;
};

#endif // CORE_H
