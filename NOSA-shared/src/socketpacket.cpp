#include "socketpacket.h"

SocketPacket::SocketPacket(NetworkSocket *socket, Packet *packet)
    : m_socket(socket), m_packet(packet)
{
}

void SocketPacket::respond(const char* text, bool close)
{
    m_socket->sendResponse(m_packet, text, close);
}

void SocketPacket::respond(std::string text, bool close)
{
    m_socket->sendResponse(m_packet, text.c_str(), close);
}
