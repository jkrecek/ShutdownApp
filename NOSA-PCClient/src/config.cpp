#include "config.h"
#include <stdlib.h>
#include <fstream>

#define FILE_NAME "config.conf"

std::string Config::getString(std::string key)
{
    iterator itr = find(key);
    if (itr != end())
        return itr->second;

    return "";
}

int Config::getInt(std::string key)
{
    std::string val = getString(key);
    if (val != "")
        return atoi(val.c_str());

    return 0;
}

float Config::getFloat(std::string key)
{
    std::string val = getString(key);
    if (val != "")
        return atof(val.c_str());

    return 0;
}

Config* Config::load()
{
    std::ifstream infile(FILE_NAME);
    if (!infile.good())
        return NULL;

    Config* conf = new Config();
    std::string line, name, value;
    std::size_t pos;
    while (std::getline(infile, line))
    {
        if ((pos = line.find('=')) == std::string::npos)
            continue;

        name = line.substr(0, pos);
        value = line.substr(pos + 1);
        conf->insert(std::pair<std::string, std::string> (name, value));
    }

    return conf;
}

void Config::save()
{
    std::ofstream outfile(FILE_NAME);

    for (iterator itr = begin(); itr != end(); ++itr)
        outfile << itr->first << "=" << itr->second << "\n";
}

void Config::setValue(std::string key, std::string value)
{
    iterator itr = this->find(key);
    if (itr != end())
        itr->second = value;
    else
        insert(std::pair<std::string, std::string> (key, value));
}
