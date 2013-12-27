#include "configuration.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

Configuration::Configuration()
{
}

Configuration* Configuration::loadFile(const char *file)
{
    Configuration* conf = new Configuration();

    std::ifstream infile(file);
    std::string line, name, value;
    std::size_t pos;
    while (std::getline(infile, line))
    {
        if ((pos = line.find('=')) == std::string::npos)
        {
            std::cout << "Wrong line format on line `" << line << "`." << std::endl;
            continue;
        }

        name = line.substr(0, pos);
        value = line.substr(pos+1);
        conf->values.insert(std::pair<std::string, std::string>(name, value));
    }

    return conf;
}


std::string Configuration::getString(std::string key)
{
    NameValueMap::iterator itr = values.find(key);
    if (itr != values.end())
        return itr->second;

    return NULL;
}

int Configuration::getInt(std::string key)
{
    std::string val = getString(key);
    return atoi(val.c_str());

}

float Configuration::getFloat(std::string key)
{
    std::string val = getString(key);
    return atof(val.c_str());
}


IpAddress Configuration::getIpAddress(std::string key)
{
    std::string val = getString(key);
    return IpAddress(val.c_str());
}
