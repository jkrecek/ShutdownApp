#ifndef IPADDRESS_H
#define IPADDRESS_H

#include <string>

class IpAddress
{
public:
    IpAddress(const char* ip_string);

    bool operator==(const IpAddress& other);

    bool isSameDomain(const IpAddress& other);

    bool isNull();

    void setNull();

    std::string getString();

private:
    int part[4];
};

#endif // IPADDRESS_H
