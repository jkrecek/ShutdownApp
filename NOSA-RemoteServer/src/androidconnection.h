#ifndef ANDROIDCONNECTION_H
#define ANDROIDCONNECTION_H

#include "baseconnection.h"

class AndroidConnection : public BaseConnection
{
public:
    AndroidConnection(NetworkSocket* _socket, std::string user, std::string pass);

    void redistributePacket(Packet* packet);
    bool handlePacket(Packet* packet);
};

#endif // ANDROIDCONNECTION_H
