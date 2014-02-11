#include "packethandler.h"
#include "servercontrol.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "helper.h"
#include <sstream>
#ifndef _WIN32
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

typedef void(PacketHandler::*Handler)(SocketPacket* soPa);

Handler commandHandlers[COMMAND_COUNT] = {
    &PacketHandler::CommandNone,            // COMMAND_NONE
    &PacketHandler::CommandTurnOff,         // COMMAND_TURN_OFF
    &PacketHandler::CommandRestart,         // COMMAND_RESTART
    &PacketHandler::CommandLogOff,          // COMMAND_LOG_OFF
    &PacketHandler::CommandLock,            // COMMAND_LOCK
    &PacketHandler::CommandSleep,           // COMMAND_SLEEP
    &PacketHandler::CommandHibernate,       // COMMAND_HIBERNATE
    &PacketHandler::CommandGetMac,          // COMMAND_GET_MAC
    &PacketHandler::CommandTorrent,         // COMMAND_TORRENT
    &PacketHandler::CommandGetVolume,       // COMMAND_GET_VOLUME
    &PacketHandler::CommandSetVolume,       // COMMAND_SET_VOLUME
    &PacketHandler::CommandNone,            // COMMAND_STATUS
    &PacketHandler::CommandNone,            // COMMAND_GET_INFO
};

void PacketHandler::handle(SocketPacket *sopa)
{
    Command command = sopa->getCommand();
    if (command < COMMAND_COUNT)
    {
        (*this.*commandHandlers[command])(sopa);
    }
}

void PacketHandler::CommandNone(SocketPacket* soPa)
{

}

void PacketHandler::CommandTurnOff(SocketPacket* soPa)
{
#ifdef _WIN32
    Helper::request_privileges(SE_SHUTDOWN_NAME);
    ExitWindowsEx(EWX_POWEROFF, SHTDN_REASON_MAJOR_APPLICATION);
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandRestart(SocketPacket* soPa)
{
#ifdef _WIN32
    Helper::request_privileges(SE_SHUTDOWN_NAME);
    ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_APPLICATION);
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandLogOff(SocketPacket* soPa)
{
#ifdef _WIN32
    Helper::request_privileges(SE_SHUTDOWN_NAME);
    ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_MAJOR_APPLICATION);
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandLock(SocketPacket* soPa)
{
#ifdef _WIN32
    LockWorkStation();
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandSleep(SocketPacket* soPa)
{
#ifdef _WIN32
    Helper::request_privileges(SE_SHUTDOWN_NAME);
    SetSystemPowerState(false, false);
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandHibernate(SocketPacket* soPa)
{
#ifdef _WIN32
    Helper::request_privileges(SE_SHUTDOWN_NAME);
    SetSystemPowerState(true, false);
    soPa->respond("OK");
#endif
}

void PacketHandler::CommandGetMac(SocketPacket* soPa)
{
    IpAddress serverIp = soPa->getParameters();
    const char * mac = soPa->getSocket()->getMAC(&serverIp);
    soPa->respond(mac);
}

void PacketHandler::CommandTorrent(SocketPacket* soPa)
{
    URLHandler handler;
    std::list<EpisodeTorrent> torrents;

    StringVector seriesStrings = Helper::getArgsByQuotation(soPa->getParameters(), false);

    soPa->respond("Start downloading page", false);

    int count = 0;
    for(StringVector::iterator itr = seriesStrings.begin(); itr != seriesStrings.end(); ++itr) {
        NVMap parameters = NVMap::fromMultiple(*itr, '|', '=');

        std::stringstream ss;
        ss << ++count << "/" << seriesStrings.size() << " ";

        soPa->respond(ss.str() + "Finding torrents for TV show '" + parameters.getString("name") + "'", false);

        std::string response = handler.loadUrl("http://kickass.to/usearch/$user%3Aeztv/?rss=1", parameters.getString("name").c_str());
        if (!response.empty())
        {
            soPa->respond(ss.str() + "Filtering result for TV show '" + parameters.getString("name") + "'", false);
            std::list<EpisodeTorrent> magnets = handler.getKickAssMagnets(response);
            filterRequiredTorrents(&torrents, &magnets, parameters.getInt("s"), parameters.getInt("e"));
            soPa->respond(ss.str() + "TV show successfully handled '" + parameters.getString("name") + "'", false);
        }
        else {
            soPa->respond(ss.str() + "Page returned wrong HTTP code for TV show '" + parameters.getString("name") + "'", false);
        }
    }

    soPa->respond("Torrents filtered, running magnet links", false);

    runTorrents(torrents);

    soPa->respond("Done", true);
}

void PacketHandler::CommandGetVolume(SocketPacket* soPa)
{
    float volume = sPCControl.getVolumeLevel();
    const char* volumeLevel = Helper::to_string(volume);
    soPa->respond(volumeLevel);
}

void PacketHandler::CommandSetVolume(SocketPacket* soPa)
{
    float value = atof(soPa->getParameters());
    sPCControl.setVolumeLevel(value);
    soPa->respond("OK");
}

int PacketHandler::filterRequiredTorrents(std::list<EpisodeTorrent> *destination, std::list<EpisodeTorrent> *source, int minSeason, int minEpisode)
{
    std::list<EpisodeTorrent> selected;
    for (std::list<EpisodeTorrent>::iterator itr = source->begin(); itr != source->end(); ++itr)
    {
        EpisodeTorrent& episode = *itr;
        if (episode.getSeason() < minSeason || episode.getEpisode() < minEpisode)
            continue;

        std::list<EpisodeTorrent>::iterator existingItr;
        for (existingItr = selected.begin(); existingItr != selected.end(); ++existingItr)
        {
            if (episode.isSameEpisode(*existingItr))
            {
                if (episode.getQuality() > (*existingItr).getQuality())
                {
                    selected.erase(existingItr);
                    selected.push_back(episode);
                }

                break;
            }
        }

        if (existingItr == selected.end())
            selected.push_back(episode);
    }

    destination->insert(destination->end(), selected.begin(), selected.end());
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
