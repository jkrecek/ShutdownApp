#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <map>
#include <string>
#include "ipaddress.h"
#include "singleton.h"
#include "nvmap.h"

class Configuration
{
public:
    static Configuration* loadFile(const char* file);

    std::string getString(std::string key);
    int getInt(std::string key);
    float getFloat(std::string key);
    IpAddress getIpAddress(std::string key);
    bool isEmpty(std::string key);

    const char* isValid();

private:
    Configuration();

    NVMap values;
};

#endif // CONFIGURATION_H
