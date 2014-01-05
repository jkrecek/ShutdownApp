#include "configuration.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

Configuration::Configuration()
{
}

Configuration* Configuration::loadFile(const char *file)
{
    std::ifstream infile(file);
    if (!infile.good())
        return NULL;

    Configuration* conf = new Configuration();
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

    return conf;
}


std::string Configuration::getString(std::string key)
{
    return values.get(key);
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
