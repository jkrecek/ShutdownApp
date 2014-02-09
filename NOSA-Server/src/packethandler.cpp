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
    std::vector<std::string> series = Helper::getArgsByQuotation(soPa->getParameters(), true);
    URLHandler handler;
    std::list<EpisodeTorrent> torrents;

    soPa->respond("Start downloading page", false);

    std::string url_start = "http://thepiratebay.org/user/eztv/";
    for (int i = 0; i < 5; ++i)
    {
        std::string url = url_start + Helper::to_string(i) + "/3";
        std::string response = handler.loadUrl(url.c_str());
        if (response.empty())
            soPa->respond("Page return wrong http code", false);
        else
        {
            soPa->respond("Page downloaded, parsing links now", false);
            std::list<EpisodeTorrent> magnets = handler.getPirateBayMagnets(response);
            torrents.insert(torrents.end(), magnets.begin(), magnets.end());
        }
    }
    soPa->respond("Parse complete", false);

    filterTorrents(series, torrents);

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
