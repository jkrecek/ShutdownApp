#ifndef NVMAP_H
#define NVMAP_H

#include <map>
#include <string>
#include "helper.h"

class NVMap : public std::map<std::string, std::string>
{
public:
    NVMap();

    void appendPairs(StringVector vector, char delim);
    void insertPair(std::string nameValue, char delim);
    void insertValue(std::string key, std::string value);
    void insertValue(std::string key, int value);
    void insertValue(std::string key, float value);

    bool contains(std::string key);

    std::string get(std::string key);
    std::string getString(std::string key, std::string defaultValue = "");
    int getInt(std::string key, int defaultValue = 0);
    float getFloat(std::string key, float defaultValue = 0.f);

    std::string toString(char separator);
};

#endif // NVMAP_H
