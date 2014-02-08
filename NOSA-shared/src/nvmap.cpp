#include "nvmap.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sstream>

#define STR_INVALID "_INVALID_"

NVMap::NVMap()
{
}

void NVMap::insertValue(std::string key, std::string value)
{
    insert(std::pair<std::string, std::string> (key, value));
}

void NVMap::insertValue(std::string key, float value)
{
    char buf[48];
    snprintf (buf, sizeof(buf), "%f", value);

    insertValue(key, buf);
}

void NVMap::insertValue(std::string key, int value)
{
    char buf[48];
    snprintf (buf, sizeof(buf), "%d", value);

    insertValue(key, buf);
}

void NVMap::insertPair(std::string nameValue, char delim)
{
    std::size_t pos;
    if ((pos = nameValue.find(delim)) == std::string::npos)
    {
        std::cout << "Wrong format to parse NameValue `" << nameValue << "`." << std::endl;
        return;
    }

    std::string name = nameValue.substr(0, pos);
    std::string value = nameValue.substr(pos+1);
    insertValue(name, value);
}

void NVMap::appendPairs(StringVector vector, char delim)
{
    for(StringVector::iterator itr = vector.begin(); itr != vector.end(); ++itr)
        insertPair(*itr, delim);
}


bool NVMap::contains(std::string key)
{
    NVMap::iterator itr = find(key);
    return itr != end();
}

std::string NVMap::get(std::string key)
{
    NVMap::iterator itr = find(key);
    if (itr != end())
        return itr->second;

    return STR_INVALID;
}

std::string NVMap::getString(std::string key, std::string defaultValue)
{
    std::string val = get(key);
    if (val != STR_INVALID)
        return val;

    return defaultValue;
}

int NVMap::getInt(std::string key, int defaultValue)
{
    std::string val = get(key);
    if (val != STR_INVALID)
        return atoi(val.c_str());

    return defaultValue;
}

float NVMap::getFloat(std::string key, float defaultValue)
{
    std::string val = get(key);
    if (val != STR_INVALID)
        return atof(val.c_str());

    return defaultValue;
}

std::string NVMap::toString(char separator)
{
    std::ostringstream ss;
    for (NVMap::iterator itr = begin(); itr != end(); ++itr)
        ss << itr->first << "=" << itr->second << separator;

    return ss.str();
}
