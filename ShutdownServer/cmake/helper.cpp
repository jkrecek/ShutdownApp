#include "helper.h"
#include <sstream>
#include <iostream>
#include <Iphlpapi.h>

#define MAC_SIZE 17

std::vector<std::string> &Helper::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> Helper::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

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
            IpAddress ipAddress(pAdapterInfo->IpAddressList.IpAddress.String);
            if (ipAddress == NULL)
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

        if (candidate)
        {
            mac_addr = (char*)malloc(MAC_SIZE);
            _snprintf_s(mac_addr, _TRUNCATE, MAC_SIZE, "%02X:%02X:%02X:%02X:%02X:%02X",
                candidate->Address[0], candidate->Address[1],
                candidate->Address[2], candidate->Address[3],
                candidate->Address[4], candidate->Address[5]);

            //std::string ipAddressString = IpAddress(pAdapterInfo->IpAddressList.IpAddress.String).getString();
        }
    }

    free(adapterInfo);
    return mac_addr;
}
