#include "configuration.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

Configuration::Configuration()
    : m_fileName(NULL), m_fileExists(false)
{
}

Configuration* Configuration::loadFile(const char *file)
{
    Configuration* conf = new Configuration();
    conf->m_fileName = file;
    std::ifstream infile(file);
    if (infile.good())
    {
        conf->m_fileExists = true;
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
            conf->values.insertValue(name, value);
        }
    }

    return conf;
}


void Configuration::saveToFile(const char *file)
{
    std::ofstream outfile(file, std::ofstream::out);
    if (outfile.good())
        for (NVMap::iterator itr = values.begin(); itr != values.end(); ++itr)
            outfile << itr->first << "=" << itr->second << "\n";
}

void Configuration::save()
{
    saveToFile(m_fileName);
}

std::string Configuration::getString(std::string key, std::string defaultValue)
{
    return values.getString(key, defaultValue);
}

int Configuration::getInt(std::string key)
{
    return values.getInt(key);
}

float Configuration::getFloat(std::string key)
{
    return values.getFloat(key);
}

IpAddress Configuration::getIpAddress(std::string key)
{
    return IpAddress(values.get(key).c_str());
}


bool Configuration::isEmpty(std::string key)
{
    return values.get(key).empty();
}

const char* Configuration::isValid()
{
    if (isEmpty("REMOTE_ADDRESS") || isEmpty("USER") || isEmpty("PASS"))
        return "You must fill all required fields, please check dist file for needed params";

    if (getString("USER").length() < 4)
        return "Your username must have atleast 5 characters";

    if (getString("PASS").length() < 5)
        return "Your password must have atleast 5 characters";

    return NULL;
}

void Configuration::setValue(std::string key, std::string value)
{
    NVMap::iterator itr = values.find(key);
    if (itr != values.end())
        itr->second = value;
    else
        values.insert(std::pair<std::string, std::string> (key, value));
}
