#ifndef PACKET_H
#define PACKET_H

#include "commandhandler.h"

class Packet
{
public:
    Packet(unsigned requestId, const char* message);
    static Packet fromBytes(char* bytes);
    char* toBytes();

    unsigned getRequestId() const { return m_requestId; }
    const char* getMessage() const { return m_message; }
    unsigned getSize() const;
    bool isCommand(Command command);
    Command getCommand();
    const char* getParameters();

    Packet responsePacket(const char* message);
    void respond(const char* message);
    const char* escape(const char* message);
private:
    Command m_command;
    unsigned m_requestId;
    const char* m_message;
};

#endif // PACKET_H
