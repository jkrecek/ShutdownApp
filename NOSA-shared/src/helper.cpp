#include "helper.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <Iphlpapi.h>
#endif

#define MAC_SIZE 17

StringVector &Helper::split(const std::string &s, char delim, StringVector &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);

    return elems;
}


StringVector Helper::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string Helper::trim(std::string s)
{
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    return s;
}

std::string Helper::toLowerCase(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

std::string Helper::replace(std::string s, const char *from, const char *to)
{
    std::size_t start_pos = 0;
    while((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, strlen(from), to);
        start_pos += strlen(to);
    }

    return s;
}

#ifdef _WIN32
std::string Helper::GetSZValueUnique( HKEY openKey, const char* regkey, const char* keyName )
{
   HKEY hKey = 0;
   BYTE data[512] ;
   DWORD szsize = 512 ;
   std::string value ;

   LONG retValue = RegOpenKeyEx( openKey, regkey, 0, KEY_READ, &hKey ) ;

   if ( retValue == ERROR_SUCCESS )
   {
        LONG retV = RegQueryValueEx( hKey, keyName, 0, 0, data, &szsize ) ;
        if ( retV == ERROR_SUCCESS )
        {
           char* _value = reinterpret_cast<char*>(data) ;
           value = _value ;

           RegCloseKey (hKey) ;
           return value ;
        }
        else
        {
        }
    }
    else
    {

    }

    RegCloseKey (hKey) ;
    return "" ;
}

const char* Helper::getMAC(IpAddress* clientIp = NULL, IpAddress* serverIp = NULL)
{
    IP_ADAPTER_INFO* adapterInfo;
    DWORD dwBufLen = sizeof(adapterInfo);
    char *mac_addr;

    adapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (adapterInfo == NULL)
        std::cerr << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << std::endl;

    if (GetAdaptersInfo(adapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(adapterInfo);
        adapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
        if (adapterInfo == NULL)
            std::cerr << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << std::endl;
    }

    if (GetAdaptersInfo(adapterInfo, &dwBufLen) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
        PIP_ADAPTER_INFO candidate = NULL;

        do {
            IpAddress ipAddress = pAdapterInfo->IpAddressList.IpAddress.String;
            if (ipAddress.isNull())
                continue;

            if (serverIp)
            {
                if (ipAddress == *serverIp)
                {
                    candidate = pAdapterInfo;
                    break;
                }
            }
            else
            {
                if (candidate)
                {
                    if (clientIp)
                    {
                        if (clientIp->isSameDomain(ipAddress))
                        {
                            candidate = pAdapterInfo;
                            break;
                        }
                    }
                }
                else
                {
                    candidate = pAdapterInfo;
                    if (!clientIp)
                        break;
                }
            }

        } while (pAdapterInfo = pAdapterInfo->Next);

        if (!candidate)
            return NULL;

        mac_addr = (char*)malloc(MAC_SIZE);
        _snprintf_s(mac_addr, _TRUNCATE, MAC_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X",
            candidate->Address[0], candidate->Address[1],
            candidate->Address[2], candidate->Address[3],
            candidate->Address[4], candidate->Address[5]);

    }

    delete adapterInfo;
    return mac_addr;
}
#endif

bool Helper::iequals(const std::string& a, const std::string& b)
{
    return a.size() == b.size() && toLowerCase(a) == toLowerCase(b);
}

const char* Helper::to_string(int val)
{
    char str[64];
    sprintf(str, "%d", val);
    return strdup(str);
}

const char* Helper::to_string(float val)
{
    char str[64];
    sprintf(str, "%f", val);
    return strdup(str);
}

const char* Helper::stripNewLine(const char* ori)
{
    if (ori[strlen(ori)-1] == '\n')
    {
    #ifdef _WIN32
        char* res = strdup(ori);
        res[strlen(res)-1] = 0;
        return res;
    #else
        return strndup(ori, strlen(ori)-1);
    #endif
    }

    return ori;
}

void Helper::printBytes(char **bytes, unsigned printMax)
{
    if (!printMax)
        printMax = strlen(*bytes);

    std::cout << "Len: " << strlen(*bytes) << ", bytes: ";
    for (int i = 0; i < printMax; ++i)
        std::cout << (int) (*bytes)[i] << " ";

    std::cout << std::endl;
}
