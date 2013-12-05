#ifndef CONNECTION_H
#define CONNECTION_H

#include <windows.h>
#include "core.h"
#include "urlhandler.h"
#include <vector>

class Connection
{
public:
    Connection(Core* core, SOCKET socket, sockaddr_in info);
    SOCKET getSocket() {
        return socket;
    }

    void handle();
    void process(const char* message);
    void respond(const char* message);
    void respondAndClose(const char* message);
    void close();

private:
    const char* safeResponseFromat(const char* message);

    std::list<EpisodeTorrent> getTorrentMagnets();
    std::vector<std::string> getArgsByQuotation(std::string arg, bool lower);
    void filterTorrents(std::vector<std::string> series, std::list<EpisodeTorrent>& torrents);
    void runTorrents(const std::list<EpisodeTorrent>& torrents);

    Core* core;
    SOCKET socket;
    sockaddr_in info;
};

#endif // CONNECTION_H
