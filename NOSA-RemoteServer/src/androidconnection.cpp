#ifdef __WIN32
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <arpa/inet.h>
#endif

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
    switch(packet->getCommand())
    {
        case COMMAND_STATUS:
        {
            PCConnection* con = sConnections.getPCConnection(this);
            socket->sendResponse(packet, con ? "ONLINE" : "OFFLINE");
            break;
        }

        case COMMAND_GET_INFO:
        {
            PCConnection* con = sConnections.getPCConnection(this);
            if (!con) {
                socket->sendResponse(packet, "ERROR_NO_CONNECTION");
                return true;
            }

            NVMap map;
            char _hostname[NI_MAXHOST];
            char _servInfo[NI_MAXSERV];

            const sockaddr_in& info = con->getSocket()->getInfo();

            char * ipAddress = NULL;
            if (info.sin_family== AF_INET)
                ipAddress = inet_ntoa(con->getSocket()->getInfo().sin_addr);
            else {
                ipAddress = Helper::ipv6_string(((sockaddr_in6*)&info)->sin6_addr);
            }

            map.insertValue("ip", ipAddress);

            if (getnameinfo((sockaddr*)&info, sizeof(sockaddr_in), _hostname, NI_MAXHOST, _servInfo, NI_MAXSERV, NI_NUMERICSERV)) {
                socket->sendResponse(packet, "ERROR_GETTING_INFO");
                return true;
            }

            std::string hostname = _hostname;
            size_t dotPos = hostname.find('.');
            if (dotPos != std::string::npos)
                hostname = hostname.substr(0, dotPos);

            map.insertValue("host", hostname);
            map.insertValue("mac", socket->getMAC());

            socket->sendResponse(packet, map.toString(' ').c_str());
            break;
        }
        default:
            return false;
    }

    return true;
}
