#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <string>
#ifdef _WIN32
    #include <windows.h>
#endif
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
    static bool iequals(const std::string& a, const std::string& b);
    static const char* to_string(int val);
    static const char* to_string(float val);
    static const char* stripNewLine(const char* ori);
    static void printBytes(char** bytes, unsigned printMax = 0);

#ifdef _WIN32
    static std::string GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName );
    static const char* getMAC(IpAddress* clientIp, IpAddress* serverIp);
#endif
};


#endif // HELPER_H
