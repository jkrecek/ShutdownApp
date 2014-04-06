#ifndef DEFINES_H
#define DEFINES_H

#ifdef MODULE_NO_LOOP
    #define RETRY_DELAY 0
    #define LOOP_LINE
    #define ON_NO_SOCKET if (false)
#else
    #define RETRY_DELAY 5
    #define LOOP_LINE while(true)
    #define ON_NO_SOCKET else
#endif

#define SOCKET_PORT 3691
#define CONFIG_FILENAME "config.conf"

#endif // DEFINES_H
