#ifndef SOCKETPACKET_H
#define SOCKETPACKET_H

#include "networksocket.h"

class SocketPacket
{
public:
    SocketPacket(NetworkSocket* socket, Packet* packet);

    void respond(const char* text, bool close = true);
    void respond(std::string text, bool close = true);

    NetworkSocket* getSocket() const { return m_socket; }
    Packet* getPacket() const { return m_packet; }

    Command getCommand() const { return m_packet->getCommand(); }
    const char* getParameters() const { return m_packet->getParameters(); }
private:
    NetworkSocket* m_socket;
    Packet* m_packet;
};

#endif // SOCKETPACKET_H
