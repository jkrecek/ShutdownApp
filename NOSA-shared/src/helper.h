#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <string>
#ifdef _WIN32
    #include <In6addr.h>
    #include <windows.h>
#else
    #include <netinet/in.h>
#endif

#include "ipaddress.h"

typedef std::vector<std::string> StringVector;
typedef unsigned int uint;
typedef unsigned long long ullint;

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
    static size_t position_of_char(const char* text, char ch);
    static void printBytes(char** bytes, unsigned printMax = 0);
    static bool file_exists(const std::string& name);
    static std::string fromDecimal(ullint n, ullint b);
    static char* strndup(const char* s, size_t n);
    static std::string getPathToFile(const char* fileName, bool extraBackslash = false);
    static char* ipv6_string(in6_addr& addr);
    static StringVector getArgsByQuotation(std::string arg, bool lower);
    static std::string getTagContent(std::string text, std::string tag, std::size_t start = 0);

#ifdef _WIN32
    static std::string GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName );
    static const char* getMAC(IpAddress* clientIp, IpAddress* serverIp);
    static bool request_privileges(LPCSTR value);
    static bool request_privileges(LPCSTR values[]);
#endif
};


#endif // HELPER_H
