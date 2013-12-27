#include "androidconnection.h"
#include "connectioncontainer.h"

AndroidConnection::AndroidConnection(NetworkSocket* _socket)
    : BaseConnection(_socket, TYPE_ANDROID)
{

}

void AndroidConnection::redistributeLine(std::string line)
{
    // TODO
    std::vector<AndroidConnection*> conns = sConnections.getAndroidConnections((PCConnection*)this);
    if (conns.empty())
        return;

    for (std::vector<AndroidConnection*>::iterator itr = conns.begin(); itr != conns.end(); ++itr)
    {
        if (this != *itr)
            (*itr)->getSocket()->sendLine(line.c_str());
    }
}
