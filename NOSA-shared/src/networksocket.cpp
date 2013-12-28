#include "networksocket.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#ifndef _WIN32
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
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
    //freeaddrinfo((sockaddr*)&info);
#else
    freeaddrinfo((addrinfo*)&info);
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
