#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "helper.h"
#include "nvmap.h"

#define BUFFER_SIZE 1000

#ifdef _WIN32
    #include <windows.h>
    typedef SOCKET TCPSocket;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    typedef int TCPSocket;
#endif

class NetworkSocket
{
public:
    NetworkSocket(TCPSocket socket);
    ~NetworkSocket();

    std::string readLine();
    NVMap parsePacket(std::string line);

    void sendLine(const char* line);

    void send(const char* message);

    void close();
    bool isOpen() { return opened; }

    int getId() { return id; }

private:
    std::string findLineInBuffer();

    TCPSocket socket;
    //char buffer[BUFFER_SIZE];
    char* buffer;
    int size;

    bool opened;
    int id;
};

#endif // TCPSOCKET_H
