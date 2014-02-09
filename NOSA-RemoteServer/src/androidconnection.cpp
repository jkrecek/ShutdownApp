#include "androidconnection.h"
#include "connectioncontainer.h"
#include "singleton.h"

AndroidConnection::AndroidConnection(NetworkSocket *_socket, std::string user, std::string pass)
    : BaseConnection(_socket, TYPE_ANDROID, user, pass)
{

}

void AndroidConnection::redistributePacket(Packet *packet)
{
    // TODO
    // rewrite request id handling

    if (PCConnection* conn = sConnections.getPCConnection(this))
        conn->getSocket()->send(*packet);
}

bool AndroidConnection::handlePacket(Packet *packet)
{
    if (packet->isCommand("STATUS"))
    {
        PCConnection* con = sConnections.getPCConnection(this);
        socket->sendResponse(packet, con ? "ONLINE" : "OFFLINE");
        return true;
    }

    return false;
}
