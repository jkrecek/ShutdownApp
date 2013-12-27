#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <string>
#include <windows.h>
#include "ipaddress.h"

typedef std::vector<std::string> StringVector;
class Helper
{
public:
    static StringVector &split(const std::string &s, char delim, StringVector &elems);
    static StringVector split(const std::string &s, char delim);
    static std::string trim(std::string s);
    static std::string toLowerCase(std::string s);
    static std::string replace(std::string s, const char* from, const char* to);
    static std::string GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName );
    static const char* getMAC(IpAddress* clientIp, IpAddress* serverIp);
    static bool iequals(const std::string& a, const std::string& b);
};


#endif // HELPER_H
