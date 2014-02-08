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
    bool saveToFile(const char* file);
    bool save();

    bool fileExists() const { return m_fileExists; }

    std::string getString(std::string key, std::string defaultValue = "");
    int getInt(std::string key);
    float getFloat(std::string key);
    IpAddress getIpAddress(std::string key);

    bool isEmpty(std::string key);
    const char* isValid();

    void setValue(std::string key, std::string value);

private:
    Configuration();

    NVMap values;
    std::string m_fileName;
    bool m_fileExists;
};

#endif // CONFIGURATION_H
