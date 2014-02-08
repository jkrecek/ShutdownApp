#include "packet.h"
#include <iostream>
#include "helper.h"

#ifdef _WIN32
    #include "winsock.h"
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#else
    #include <netinet/in.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
#endif

Packet::Packet(unsigned requestId, const char *message)
    : m_requestId(requestId), m_message(message)
{

}

Packet::Packet()
{

}

Packet Packet::fromBytes(char *bytes)
{
    char intBuffer[sizeof(uint32_t)];
    memcpy(intBuffer, bytes, sizeof(uint32_t));

    Packet packet;
    packet.m_requestId = ntohl(*(uint32_t*)intBuffer);
    packet.m_message = strdup(bytes + sizeof(uint32_t));
    delete[] bytes;
    return packet;
}

char* Packet::toBytes()
{
    uint32_t req_end = htonl(m_requestId);
    char * bytes = new char[getSize()];
    memcpy(bytes, (char*)&req_end, sizeof(uint32_t));

    memcpy(bytes + sizeof(uint32_t) , m_message, strlen(m_message));
    bytes[getSize() - 1] = '\n';

    return bytes;
}

unsigned Packet::getSize() const
{
    return sizeof(uint32_t) + strlen(m_message) + 1;
}

const char* Packet::escape(const char* message)
{
    message = Helper::replace(message, "\n", "\\n").c_str();
    return message;
}

Packet Packet::responsePacket(const char *message)
{
    Packet packet;
    packet.m_requestId = m_requestId;
    packet.m_message = message;
    return packet;
}

bool Packet::isCommand(const char *command) {
    const char* cmd = getCommand();
    return cmd && strcmp(cmd, command) == 0;
}

const char* Packet::getCommand()
{
    size_t position = Helper::position_of_char(m_message, ' ');
    if (position != std::string::npos)
        return Helper::strndup(m_message, position);

    return m_message;
}

const char* Packet::getParameters()
{
    char* end = (char*)memchr(m_message, ' ', strlen(m_message));
    if (!end)
        return NULL;

    return strdup(end+1);
}

