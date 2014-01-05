#ifndef PCCONNECTION_H
#define PCCONNECTION_H

#include "baseconnection.h"

class PCConnection : public BaseConnection
{
public:
    PCConnection(NetworkSocket* _socket);

    void redistributePacket(Packet* packet);
    bool handlePacket(Packet* packet);
};

#endif // PCCONNECTION_H
