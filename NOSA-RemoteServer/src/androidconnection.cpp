#include "androidconnection.h"
#include "connectioncontainer.h"
#include "singleton.h"

AndroidConnection::AndroidConnection(NetworkSocket* _socket)
    : BaseConnection(_socket, TYPE_ANDROID)
{

}

void AndroidConnection::redistributeLine(std::string line)
{
    if (PCConnection* conn = sConnections.getPCConnection(this))
        conn->getSocket()->sendLine(line.c_str());
}
