#include "connection.h"
#include <string>
#include <list>
#include "servercontrol.h"
#include "core.h"
#include <windows.h>
#include <iostream>
#include "helper.h"

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
    char* rest = NULL;
    std::string command = strtok_s((char*)message, " ", &rest);

    Command controlCommand;
    if ((controlCommand = core->getServerControl()->getCommand(command)) != NONE)
    {
        responseClose(core->getServerControl()->execute(controlCommand));
        return;
    }

    if (command == "GET_MAC")
    {
        IpAddress serverIp = rest ? IpAddress(rest) : NULL;
        IpAddress clientIp = inet_ntoa((in_addr)info.sin_addr);
        const char * mac = Helper::getMAC(&clientIp, &serverIp);
        if (mac == "") {
            mac = Helper::getMAC(NULL, NULL);
            if (mac == "")
                mac = "ERROR";
        }

        responseClose(mac);
        return;
    }
}

void Connection::respond(const char *message)
{
    size_t size;
    if ((size = send(socket, message, strlen(message), 0)) == SOCKET_ERROR)
    {
        std::cerr << "Could not send data" << std::endl;
    }
}

void Connection::responseClose(const char *message)
{
    respond(message);
    close();
}

void Connection::close()
{
    closesocket(socket);
}
