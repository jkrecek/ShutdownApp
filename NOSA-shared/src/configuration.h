#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <map>
#include <string>
#include "ipaddress.h"

typedef std::map<std::string, std::string> NameValueMap;

class Configuration
{
public:
    static Configuration* loadFile(const char* file);

    std::string getString(std::string key);
    int getInt(std::string key);
    float getFloat(std::string key);
    IpAddress getIpAddress(std::string key);

private:
    Configuration();

    NameValueMap values;
};

#endif // CONFIGURATION_H
