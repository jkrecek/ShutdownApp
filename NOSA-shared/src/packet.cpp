#include "packet.h"
#include <iostream>
#include "helper.h"

#ifdef _WIN32
    #include "winsock.h"
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#endif

Packet::Packet(unsigned requestId, const char *message)
    : m_requestId(requestId), m_message(message)
{

}

Packet::Packet()
{

}

Packet* Packet::fromBytes(char *bytes)
{
    char* intBuffer = (char*)malloc(sizeof(uint32_t));
    memcpy(intBuffer, bytes, sizeof(uint32_t));

    Packet* packet = new Packet();
    packet->m_requestId = ntohl(*(uint32_t*)intBuffer);
    packet->m_message = bytes + sizeof(uint32_t);
    return packet;
}

char* Packet::toBytes()
{
    uint32_t req_end = htonl(m_requestId);
    char* bytes = (char*)malloc(getSize());
    memcpy(bytes, (char*)&req_end, sizeof(uint32_t));


    memcpy(bytes + sizeof(uint32_t) , m_message, strlen(m_message));
    bytes[getSize() - 2] = '\n';
    bytes[getSize() - 1] = 0;

    return bytes;
}

unsigned Packet::getSize() const
{
    return sizeof(uint32_t) + strlen(m_message) + 2;
}

const char* Packet::escape(const char* message)
{
    message = Helper::replace(message, "\n", "\\n").c_str();
    return message;
}

Packet* Packet::responsePacket(const char *message)
{
    Packet* packet = new Packet();
    packet->m_requestId = m_requestId;
    packet->m_message = message;
    return packet;
}

