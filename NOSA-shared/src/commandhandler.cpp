#include "commandhandler.h"

CommandHandler::CommandHandler()
{
    cmd = COMMAND_NONE;

    addCommand(NULL);               // COMMAND_NONE
    addCommand("TURN_OFF");         // COMMAND_TURN_OFF
    addCommand("RESTART");          // COMMAND_RESTART
    addCommand("LOG_OFF");          // COMMAND_LOG_OFF
    addCommand("LOCK");             // COMMAND_LOCK
    addCommand("SLEEP");            // COMMAND_SLEEP
    addCommand("HIBERNATE");        // COMMAND_HIBERNATE
    addCommand("GET_MAC");          // COMMAND_GET_MAC
    addCommand("TORRENT");          // COMMAND_TORRENT
    addCommand("GET_VOLUME");       // COMMAND_GET_VOLUME
    addCommand("SET_VOLUME");       // COMMAND_SET_VOLUME
    addCommand("STATUS");           // COMMAND_STATUS,
    addCommand("GET_INFO");         // COMMAND_GET_INFO,
}

void CommandHandler::addCommand(const char* key)
{
    if (key)
        operator[](key) = cmd;

    cmd = (Command)(cmd + 1);
}

Command CommandHandler::getCommand(std::string str)
{
    CommandHandler::iterator itr = find(str);
    if (itr == end())
        return COMMAND_NONE;

    return itr->second;
}
