#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

class Config : public std::map<std::string, std::string>
{
public:
    static Config* load();
    void save();
    void setValue(std::string key, std::string value);

    std::string getString(std::string key);
    int getInt(std::string key);
    float getFloat(std::string key);
};

#endif // CONFIG_H
