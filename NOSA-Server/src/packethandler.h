#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "mainsocket.h"
#include <list>
#include "urlhandler.h"
#include "socketpacket.h"

class PacketHandler
{
public:
    PacketHandler() { }

    void handle(SocketPacket *sopa);

    void CommandNone(SocketPacket* soPa);
    void CommandTurnOff(SocketPacket* soPa);
    void CommandRestart(SocketPacket* soPa);
    void CommandLogOff(SocketPacket* soPa);
    void CommandLock(SocketPacket* soPa);
    void CommandSleep(SocketPacket* soPa);
    void CommandHibernate(SocketPacket* soPa);
    void CommandGetMac(SocketPacket* soPa);
    void CommandTorrent(SocketPacket* soPa);
    void CommandGetVolume(SocketPacket* soPa);
    void CommandSetVolume(SocketPacket* soPa);

private:
    std::list<EpisodeTorrent> getTorrentMagnets(Packet* packet);
    void filterTorrents(std::vector<std::string> series, std::list<EpisodeTorrent>& torrents);
    void runTorrents(const std::list<EpisodeTorrent>& torrents);
};

#endif // PACKETHANDLER_H
