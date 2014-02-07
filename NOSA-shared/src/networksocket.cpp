#include "networksocket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "socketclosedexception.h"
#include "packet.h"

#ifdef _WIN32
    #include <winsock.h>
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <arpa/inet.h>
#endif

NetworkSocket::NetworkSocket(TCPSocket _socket, sockaddr_in& _info)
    : socket(_socket), info(_info), size(0), bufferPtr(buffer)
{

}

NetworkSocket::~NetworkSocket()
{
    doClose();
}

Packet NetworkSocket::readPacket()
{
    char* bytes = readLine();
    Packet packet = Packet::fromBytes(bytes);
    std::cout << "RCV[" << getSocketId() << "]: `" << packet.getMessage() << "`" << std::endl;
    return packet;
}

char* NetworkSocket::readLine()
{
    char* line = NULL;
    if (size > 0)
    {
        line = readBuffer();
        if (size || line[strlen(line)-1] == '\n')
            return line;
    } else
        bufferPtr = buffer;

    char* rest = NULL;
    do {
        size = recv(socket, bufferPtr, BUFFER_SIZE - 1, 0);
        if (size <= 0)
            throw SocketClosedException();

        rest = readBuffer();
    } while(!rest);

    if (line == NULL)
        line = rest;
    else
    {
        std::vector<char> v;
        v.insert(v.end(), line, line + strlen(line));
        v.insert(v.end(), rest, rest + strlen(rest));
        line = v.data();
    }

    return line;
}

char* NetworkSocket::readBuffer()
{
    char* end = (char*)memchr(bufferPtr, '\n', size);
    if (!end)
        return NULL;

    unsigned len = end - bufferPtr;
    if (len < 5)
        return NULL;

    char* res = new char[len + 1];
    memcpy(res, bufferPtr, len);
    res[len] = 0;
    size -= len + 2;
    bufferPtr = end + 1;
    return res;
}

NVMap NetworkSocket::parsePacket(std::string line)
{
    NVMap nvmap;
    StringVector parts = Helper::split(line, ' ');
    nvmap.appendPairs(parts, '=');
    return nvmap;
}

void NetworkSocket::send(Packet packet)
{
    size_t size;
    char * bytes = packet.toBytes();
    if ((size = ::send(socket, bytes, packet.getSize(), 0)) == SOCKET_ERROR)
        std::cout << "SND[" << getSocketId() << "]: E: Could not send data: `" << packet.getMessage() << "`" << std::endl;
    else
        std::cout << "SND[" << getSocketId() << "]: `" << packet.getMessage() << "`" << std::endl;

    delete[] bytes;
}

void NetworkSocket::sendMsg(const char *message)
{
    send(Packet(0, message));
}

void NetworkSocket::sendResponse(Packet *originalPacket, const char *response, bool close)
{
    send(originalPacket->responsePacket(response));
    if (close)
    {
        send(originalPacket->responsePacket("CLOSE"));
    }
}

void NetworkSocket::close()
{
    sendMsg("CLOSE");
    doClose();
}

void NetworkSocket::doClose()
{
    std::cout << "Closing socket: " << getSocketId() << std::endl;
    shutdown(socket, 0);
#ifdef _WIN32
    closesocket(socket);
#else
    ::close(socket);
#endif
    socket = 0;
}

const char* NetworkSocket::getMAC(IpAddress *targetIp)
{
    const char* mac_addr;
#ifdef _WIN32
    IpAddress clientIp = inet_ntoa((in_addr)info.sin_addr);
    mac_addr = Helper::getMAC(&clientIp, targetIp);

    if (!mac_addr)
        mac_addr = Helper::getMAC(NULL, NULL);
#else
    ifreq ifr;
    ifconf ifc;
    char buf[1024];
    bool success = false;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(socket, SIOCGIFCONF, &ifc);

    ifreq* it = ifc.ifc_req;
    const ifreq* const end = it + (ifc.ifc_len / sizeof(ifreq));

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(socket, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
                if (ioctl(socket, SIOCGIFHWADDR, &ifr) == 0) {
                    success = true;
                    break;
                }
            }
        }
    }

    char* mac = new char[17];
    sprintf(mac, " %02x:%02x:%02x:%02x:%02x:%02x",
        (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    mac_addr = mac;
#endif
    if (!mac_addr)
        mac_addr = "ERROR";

    return mac_addr;

}
