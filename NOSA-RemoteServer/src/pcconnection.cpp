#include "pcconnection.h"
#include "connectioncontainer.h"

PCConnection::PCConnection(NetworkSocket* _socket)
    : BaseConnection(_socket, TYPE_PC)
{

}


void PCConnection::redistributeLine(std::string line)
{
    std::vector<AndroidConnection*> conns = sConnections.getAndroidConnections(this);
    if (conns.empty())
        return;

    for (std::vector<AndroidConnection*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
    {
        (*itr)->getSocket()->send(line.c_str());
    }
}
