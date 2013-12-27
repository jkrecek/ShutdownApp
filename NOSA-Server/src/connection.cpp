#include "connection.h"
#include <string>
#include <list>
#include "servercontrol.h"
#include "core.h"
#include <windows.h>
#include <iostream>
#include "helper.h"
#include "urlhandler.h"
#include <algorithm>
#include <map>

#define BUFFER_SIZE 1000

Connection::Connection(Core* _core, SOCKET _socket, sockaddr_in _info)
    : core(_core), socket(_socket), info(_info)
{

}

void Connection::handle()
{
    std::string text;
    char buffer[BUFFER_SIZE];
    size_t size;
    std::list<std::string> messages;
    do
    {
        size = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if (size > 0)
        {
            char *start = buffer;
            char *end = NULL;
            while (end = (char*)memchr(start, '\n', size))
            {
                text.append(start, end - start);
                messages.push_back(text.c_str());
                text.clear();
                size -= end - start;
                start = end + 1;
            }

            if (size > 0)
                text.append(start, size);
        }
    } while (text.empty());

    for (std::list<std::string>::iterator itr = messages.begin(); itr != messages.end(); ++itr)
        process((*itr).c_str());

}

void Connection::process(const char* message)
{
    std::string mesStr = message;
    std::size_t firstSpace = mesStr.find(" ");
    std::string command , arguments;
    if (firstSpace == std::string::npos)
        command = mesStr;
    else
    {
        command = mesStr.substr(0, firstSpace);
        arguments = mesStr.substr(firstSpace);
    }

    std::cout << "I: Incoming command: '" << command << "'" << std::endl;

    Command controlCommand;
    if ((controlCommand = core->getServerControl()->getCommand(command)) != NONE)
    {
        respondAndClose(core->getServerControl()->execute(controlCommand));
        return;
    }

    if (command == "GET_MAC")
    {
        IpAddress serverIp = !arguments.empty() ? IpAddress(arguments.c_str()) : NULL;
        IpAddress clientIp = inet_ntoa((in_addr)info.sin_addr);
        const char * mac = Helper::getMAC(&clientIp, &serverIp);
        if (mac == "") {
            mac = Helper::getMAC(NULL, NULL);
            if (mac == "")
                mac = "ERROR";
        }

        respondAndClose(mac);
        return;
    }

    if (command == "TORRENT")
    {
        std::vector<std::string> series = getArgsByQuotation(arguments, true);
        std::list<EpisodeTorrent> torrents = getTorrentMagnets();

        filterTorrents(series, torrents);

        respond("Torrents filtered, running magnet links");

        runTorrents(torrents);

        respondAndClose("Done");
        return;
    }

    if (command == "GET_VOLUME")
    {
        char chararray[6];
        ServerControl* control = core->getServerControl();
        float volume = control->getVolumeLevel();
        snprintf (chararray, 7, "%1.4f", volume);
        respondAndClose(&chararray[0]);

        return;
    }

    if (command == "SET_VOLUME")
    {
        float value = atof(arguments.c_str());
        ServerControl* control = core->getServerControl();
        control->setVolumeLevel(value);
        respondAndClose("OK");
        return;
    }
}

void Connection::respond(const char *message)
{
    size_t size;
    message = safeResponseFromat(message);
    if ((size = send(socket, message, strlen(message), 0)) == SOCKET_ERROR)
    {
        std::string errMsg = message;
        errMsg = errMsg.substr(0, errMsg.size() - 1);
        std::cout << "W: Could not send data: `" << errMsg << "`" << std::endl;
    }
}

void Connection::respondAndClose(const char *message)
{
    respond(message);
    close();
}

void Connection::close()
{
    const char* closeStr = "CLOSE\n";
    send(socket, closeStr, strlen(closeStr), 0);
    closesocket(socket);
}

const char *Connection::safeResponseFromat(const char *message)
{
    std::string str = message;
    str = Helper::replace(str, "\n", "\\n");
    str += "\n";
    return str.c_str();
}

std::list<EpisodeTorrent> Connection::getTorrentMagnets()
{
    URLHandler handler;
    respond("Start downloading page");

    std::list<EpisodeTorrent> list;
    /*std::string response = handler.loadUrl("http://eztv.it");
    if (response.empty())
    {
        responseClose("Page return wrong http code");
        return;
    }

    respond("Page downloaded, parsing links now");
    list = handler.getEztvMagnets(response);*/

    std::string url_start = "http://thepiratebay.pe/user/eztv/";
    char buffer[2];
    for (int i = 0; i < 5; ++i)
    {
        std::string url = url_start + itoa(i, buffer, 10) + "/3";
        std::string response = handler.loadUrl(url.c_str());
        if (response.empty())
            respondAndClose("Page return wrong http code");
        else
        {
            respond("Page downloaded, parsing links now");
            std::list<EpisodeTorrent> magnets = handler.getPirateBayMagnets(response);
            list.insert(list.end(), magnets.begin(), magnets.end());
        }
    }
    respond("Parse complete");

    return list;
}

std::vector<std::string> Connection::getArgsByQuotation(std::string arg, bool lower = false)
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

void Connection::filterTorrents(std::vector<std::string> series, std::list<EpisodeTorrent> &torrents)
{
    std::vector<std::string>::iterator it;
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

void Connection::runTorrents(const std::list<EpisodeTorrent> &torrents)
{
    std::string magnetAppPath = Helper::GetSZValueUnique( HKEY_CLASSES_ROOT, "Magnet\\shell\\open\\command\\", "");

    for(std::list<EpisodeTorrent>::const_iterator itr = torrents.begin(); itr != torrents.end(); ++itr)
    {
        std::string command = "start " + Helper::replace(magnetAppPath, "%1", (*itr).getMagnet().c_str());
        system(command.c_str());
    }
}
