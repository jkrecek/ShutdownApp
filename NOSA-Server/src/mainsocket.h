#ifndef MAINSOCKET_H
#define MAINSOCKET_H

#include "networksocket.h"
#include "configuration.h"

class MainSocket : public NetworkSocket
{
public:
    MainSocket(TCPSocket socket, sockaddr_in info);
    static MainSocket* createSocket(Configuration* configuration);
    static const char* generateAuthMessage(Configuration* configuration);
};

#endif // MAINSOCKET_H
