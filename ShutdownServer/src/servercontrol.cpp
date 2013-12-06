#include "servercontrol.h"
#include <iostream>

#define COMMAND_TURN_OFF "shutdown /s /f /t 0"
#define COMMAND_RESTART "shutdown /r /f /t 0"
#define COMMAND_LOCK "rundll32.exe user32.dll,LockWorkStation"
#define COMMAND_SLEEP "rundll32 powrprof.dll,SetSuspendState Standby"

ServerControl::ServerControl()
{
}

const char* ServerControl::execute(Command command)
{
    const char* sys_command = NULL;
    switch(command) {
        case POWER_OFF: sys_command = COMMAND_TURN_OFF; break;
        case RESTART:   sys_command = COMMAND_RESTART;  break;
        case LOCK:      sys_command = COMMAND_LOCK;     break;
        case SLEEP:     sys_command = COMMAND_SLEEP;    break;
        default:        sys_command = NULL;             break;
    }

    if (sys_command)
    {
        system(sys_command);
        return "OK";
    }
    else
    {
        std::cerr << "W: Unable to find proper command for user action." << std::endl;
        return "ERROR";
    }

}

Command ServerControl::getCommand(std::string string)
{
    if (string == "TURN_OFF")
        return POWER_OFF;
    else if (string == "RESTART")
        return RESTART;
    else if (string == "LOCK")
        return LOCK;
    else if (string == "SLEEP")
        return SLEEP;

    return NONE;
}
