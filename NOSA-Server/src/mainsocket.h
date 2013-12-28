#ifndef MAINSOCKET_H
#define MAINSOCKET_H

#include "networksocket.h"

class MainSocket : public NetworkSocket
{
public:
    MainSocket(TCPSocket socket, sockaddr_in info);
    static MainSocket* createSocket(std::string host, int port);
};

#endif // MAINSOCKET_H
