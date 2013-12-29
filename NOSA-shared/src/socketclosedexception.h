#ifndef NOSAEXCEPTION_H
#define NOSAEXCEPTION_H

#include <exception>

class SocketClosedException : public std::exception
{
public:
    SocketClosedException();
};

#endif // NOSAEXCEPTION_H
