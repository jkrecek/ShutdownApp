#include "packethandler.h"
#include "servercontrol.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"
#ifndef _WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif



PacketHandler::PacketHandler(MainSocket *_socket)
    : socket(_socket)
{
}

void PacketHandler::accepted(Packet *packet)
{
    /*std::string line = packet->getMessage();
    std::size_t firstSpace = line.find(" ");
    std::string command , arguments;
    if (firstSpace == std::string::npos)
        command = line;
    else
    {
        command = line.substr(0, firstSpace);
        arguments = line.substr(firstSpace);
    }*/

    Command controlCommand = sPCControl.getCommand(packet->getCommand());
    if (controlCommand != NONE)
    {
        const char* result = sPCControl.execute(controlCommand);
        socket->sendResponse(packet, result);
        return;
    }

    if (packet->isCommand("GET_MAC"))
    {
        IpAddress serverIp = packet->getParameters();
        const char * mac = socket->getMAC(&serverIp);
        socket->sendResponse(packet, mac);
        return;
    }

    if (packet->isCommand("TORRENT"))
    {
        std::vector<std::string> series = getArgsByQuotation(packet->getParameters(), true);
        std::list<EpisodeTorrent> torrents = getTorrentMagnets(packet);

        filterTorrents(series, torrents);

        socket->sendResponse(packet, "Torrents filtered, running magnet links", false);

        runTorrents(torrents);

        socket->sendResponse(packet, "Done", true);

        return;
    }

    if (packet->isCommand("GET_VOLUME"))
    {
        float volume = sPCControl.getVolumeLevel();
        const char* volumeLevel = Helper::to_string(volume);
        socket->sendResponse(packet, volumeLevel);

        return;
    }

    if (packet->isCommand("SET_VOLUME"))
    {
        float value = atof(packet->getParameters());
        sPCControl.setVolumeLevel(value);
        socket->sendResponse(packet, "OK");
        return;
    }
}

std::list<EpisodeTorrent> PacketHandler::getTorrentMagnets(Packet* packet)
{
    URLHandler handler;
    std::list<EpisodeTorrent> list;

    socket->sendResponse(packet, "Start downloading page", false);

    std::string url_start = "http://thepiratebay.pe/user/eztv/";
    for (int i = 0; i < 5; ++i)
    {
        std::string url = url_start + Helper::to_string(i) + "/3";
        std::string response = handler.loadUrl(url.c_str());
        if (response.empty())
            socket->sendResponse(packet, "Page return wrong http code", false);
        else
        {
            socket->sendResponse(packet, "Page downloaded, parsing links now", false);
            std::list<EpisodeTorrent> magnets = handler.getPirateBayMagnets(response);
            list.insert(list.end(), magnets.begin(), magnets.end());
        }
    }
    socket->sendResponse(packet, "Parse complete", false);

    return list;
}

std::vector<std::string> PacketHandler::getArgsByQuotation(std::string arg, bool lower = false)
{
    std::vector<std::string> v;
    for (std::size_t st = 0, en = 0; (st = arg.find('"', st)) != std::string::npos; st = en + 1)
    {
        en = arg.find('"', ++st);
        if (lower)
            v.push_back(Helper::toLowerCase(arg.substr(st, en - st)));
        else
            v.push_back(arg.substr(st, en - st-1));
    }

    return v;
}

void PacketHandler::filterTorrents(std::vector<std::string> series, std::list<EpisodeTorrent> &torrents)
{
    StringVector::iterator it;
    for (std::list<EpisodeTorrent>::iterator itr = torrents.begin(); itr != torrents.end(); )
    {
        const std::string& title = Helper::toLowerCase((*itr).getTitle());
        if ((it = std::find(series.begin(), series.end(), title)) == series.end())
        {
            itr = torrents.erase(itr);
            continue;
        }

        bool isBeingRemoved = false;
        for (std::list<EpisodeTorrent>::iterator altItr = torrents.begin(); altItr != itr; ++altItr)
        {
            if (!(*itr).isSameEpisode(*altItr))
                continue;

            if ((*itr).getQuality() > (*altItr).getQuality())
                torrents.erase(altItr);
            else
                itr = torrents.erase(itr);

            isBeingRemoved = true;
            break;
        }

        if (!isBeingRemoved)
            ++itr;
    }
}

void PacketHandler::runTorrents(const std::list<EpisodeTorrent> &torrents)
{
#ifdef _WIN32
    std::string magnetAppPath = Helper::GetSZValueUnique( HKEY_CLASSES_ROOT, "Magnet\\shell\\open\\command\\", "");
#else
    // TODO
    std::string magnetAppPath = "";
#endif

    for(std::list<EpisodeTorrent>::const_iterator itr = torrents.begin(); itr != torrents.end(); ++itr)
    {
        std::string command = "start " + Helper::replace(magnetAppPath, "%1", (*itr).getMagnet().c_str());
        system(command.c_str());
    }
}
