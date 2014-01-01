#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "mainsocket.h"
#include <list>
#include "urlhandler.h"

class PacketHandler
{
public:
    PacketHandler(MainSocket* socket);

    void accepted(Packet* packet);

private:
    std::list<EpisodeTorrent> getTorrentMagnets(Packet* packet);
    std::vector<std::string> getArgsByQuotation(std::string arg, bool lower);
    void filterTorrents(std::vector<std::string> series, std::list<EpisodeTorrent>& torrents);
    void runTorrents(const std::list<EpisodeTorrent>& torrents);

    MainSocket *socket;
};

#endif // PACKETHANDLER_H
