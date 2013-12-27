#ifndef PCCONNECTION_H
#define PCCONNECTION_H

#include "baseconnection.h"

class PCConnection : public BaseConnection
{
public:
    PCConnection(NetworkSocket* _socket);

    void redistributeLine(std::string line);
};

#endif // PCCONNECTION_H
