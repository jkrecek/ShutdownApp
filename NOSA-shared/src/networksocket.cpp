#include "networksocket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#ifdef _WIN32
    #include <winsock.h>
    #include <windows.h>
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
#endif

NetworkSocket::NetworkSocket(TCPSocket _socket, sockaddr_in _info)
    : socket(_socket), info(_info), size(0), opened(true)
{
    buffer = (char*) malloc (BUFFER_SIZE);
}

NetworkSocket::~NetworkSocket()
{
    close();
}

std::string NetworkSocket::readLine()
{
    std::string line;
    if (size)
    {
        line = findLineInBuffer();
        const char* l = line.c_str();
        if (size || l[strlen(l)-1] == '\n')
            return l;
    }

    size = recv(socket, buffer, BUFFER_SIZE - 1, 0);
    line.append(findLineInBuffer());
    std::cout << "RCV[" << getSocketId() << "]: `" << line << "`" << std::endl;
    return line.c_str();
}


std::string NetworkSocket::findLineInBuffer()
{
    if (size > 0)
    {
        if (char* end = (char*)memchr(buffer, '\n', size))
        {
            std::string res = std::string(buffer, end - buffer);
            size -= end - buffer + 1;
            buffer = end + 1;
            return res.c_str();
        }
    } else
        std::cout << "Wrong size when reading from buffer! (" << size << ")" << std::endl;

    return std::string();
}

NVMap NetworkSocket::parsePacket(std::string line)
{
    NVMap nvmap;
    StringVector parts = Helper::split(line, ' ');
    nvmap.appendPairs(parts, '=');
    return nvmap;
}

void NetworkSocket::sendLine(const char* line)
{
    bool hasNewLine = line[strlen(line)-1] == '\n';
    send(safeResponseFromat(line, !hasNewLine).c_str());
}

void NetworkSocket::send(const char* message)
{
    if (!message || !strlen(message))
        return;

    size_t size;
    if ((size = ::send(socket, message, strlen(message), 0)) == SOCKET_ERROR)
    {
        std::string errMsg = message;
        errMsg = errMsg.substr(0, errMsg.size() - 1);
        std::cout << "W: Could not send data: `" << errMsg << "`" << std::endl;
    }
    std::cout << "SND[" << getSocketId() << "]: `" << Helper::stripNewLine(message) << "`" << std::endl;
}

void NetworkSocket::close()
{
    const char* closeStr = "CLOSE\n";
    ::send(socket, closeStr, strlen(closeStr)+1, 0);
    doClose();
}

void NetworkSocket::doClose()
{
#ifdef _WIN32
    closesocket(socket);
#else
    ::close(socket);
#endif
    opened = false;
}

std::string NetworkSocket::safeResponseFromat(std::string message, bool appendNewLine)
{
    message = Helper::replace(message, "\n", "\\n");
    if (appendNewLine)
        message += "\n";

    return message;
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

    char* mac = (char*)malloc(17);
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
