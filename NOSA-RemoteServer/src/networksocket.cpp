#include "networksocket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>

NetworkSocket::NetworkSocket(TCPSocket _socket)
    : socket(_socket), size(0), opened(true)
{
    buffer = (char*) malloc (BUFFER_SIZE);
    static int sId = 0;
    ++sId;
    id = sId;
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

    std::cout << "Waiting for inc packet [" << id << "]" << std::endl;
    size = recv(socket, buffer, BUFFER_SIZE - 1, 0);
    line.append(findLineInBuffer());
    //std::string copy = line;
    std::cout << "RCV[" << id << "]:" << line << std::endl;
    return line.c_str();
}


std::string NetworkSocket::findLineInBuffer()
{
    std::string res;
    if (char* end = (char*)memchr(buffer, '\n', size))
    {
        res = std::string(buffer, end - buffer);
        size -= end - buffer + 1;
        buffer = end + 1;
        return res.c_str();
    }

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
    if (line[strlen(line)-1] == '\n')
        send(line);
    else
    {
        std::string l = line;
        l += '\n';
        send(l.c_str());
    }
}

void NetworkSocket::send(const char* message)
{
    size_t size;
    std::cout << "SND[" << id << "]:" << message << std::endl;
    if ((size = ::send(socket, message, strlen(message), 0)) == SOCKET_ERROR)
    {
        std::string errMsg = message;
        errMsg = errMsg.substr(0, errMsg.size() - 1);
        std::cout << "W: Could not send data: `" << errMsg << "`" << std::endl;
    }
    std::cout << "SND[" << id << "][OK]" << std::endl;
}

void NetworkSocket::close()
{
    const char* closeStr = "CLOSE\n";
    ::send(socket, closeStr, strlen(closeStr), 0);
    closesocket(socket);
    opened = false;
}
