#ifndef DEFINES_H
#define DEFINES_H

#ifdef MODULE_NO_LOOP
    #define RETRY_DELAY 0
    #define LOOP_LINE
    #define NO_SOCKET if (false)
#else
    #define RETRY_DELAY 15
    #define LOOP_LINE while(true)
    #define ON_NO_SOCKET else
#endif

#endif // DEFINES_H
