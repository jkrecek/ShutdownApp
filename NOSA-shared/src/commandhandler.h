#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <tr1/unordered_map>
#include <string>

enum Command {
    COMMAND_NONE = 0,

    COMMAND_TURN_OFF,
    COMMAND_RESTART,
    COMMAND_LOG_OFF,
    COMMAND_LOCK,
    COMMAND_SLEEP,
    COMMAND_HIBERNATE,
    COMMAND_GET_MAC,
    COMMAND_TORRENT,
    COMMAND_GET_VOLUME,
    COMMAND_SET_VOLUME,
    COMMAND_STATUS,
    COMMAND_GET_INFO,

    COMMAND_COUNT
};

class CommandHandler  : public std::tr1::unordered_map<std::string, Command>
{
public:
    CommandHandler();
    Command getCommand(std::string str);

private:    
    void addCommand(const char* key);

    Command cmd;
};

#endif // COMMANDHANDLER_H
