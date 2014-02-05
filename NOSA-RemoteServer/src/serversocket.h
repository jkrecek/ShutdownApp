#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "networksocket.h"

class ServerSocket : public NetworkSocket
{
public:
    ServerSocket(TCPSocket socket, sockaddr_in info);
    static ServerSocket* createSocket(int port);

    NetworkSocket* acceptConnection();

    void close();
};

#endif // SERVERSOCKET_H
