#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "helper.h"
#include "nvmap.h"

#define BUFFER_SIZE 1000

#ifdef _WIN32
    #include <windows.h>
    typedef SOCKET TCPSocket;
    typedef int SocketLength;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    typedef int TCPSocket;
    typedef socklen_t SocketLength;
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR -1
#endif

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET 0
#endif

class NetworkSocket
{
public:
    NetworkSocket(TCPSocket socket, sockaddr_in info);
    ~NetworkSocket();

    std::string readLine();
    NVMap parsePacket(std::string line);

    void sendLine(const char* line);

    void close();

    std::string safeResponseFromat(std::string message, bool appendNewLine);

    bool isOpen() { return opened; }
    int getSocketId() { return socket; }
    TCPSocket getSocket() { return socket; }
    const sockaddr_in& getInfo() const  { return info; }

    const char* getMAC(IpAddress* targetIp);

protected:
    std::string findLineInBuffer();

    void send(const char* message);

    void doClose();

    TCPSocket socket;
    sockaddr_in info;

    char* buffer;
    int size;

    bool opened;
};

#endif // TCPSOCKET_H
