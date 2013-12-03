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
    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    static std::vector<std::string> split(const std::string &s, char delim);
    static std::string GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName );
    static const char* getMAC(IpAddress* clientIp, IpAddress* lookup_ip);
};

#endif // HELPER_H
