#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#include <string>

enum Command {
    NONE,
    POWER_OFF,
    RESTART,
    LOCK,
    SLEEP
};

class ServerControl
{
public:
    ServerControl();

    const char* execute(Command command);
    Command getCommand(std::string string);
};

#endif // SERVERCONTROL_H
