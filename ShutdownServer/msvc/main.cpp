#include <iostream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <cstdio>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 1000

#define COMMAND_TURN_OFF "shutdown /s /f /t 0"
#define COMMAND_RESTART "shutdown /r /f /t 0"
#define COMMAND_LOCK "rundll32.exe user32.dll,LockWorkStation"
#define COMMAND_SLEEP "rundll32 powrprof.dll,SetSuspendState Standby"

using namespace std;

struct IpAddress
{
public:
	IpAddress(const char* ip_string)
	{
		try
		{
			char* rest = NULL;
			int counter = 0;

			char* split = strtok_s((char*)ip_string, ".", &rest);
			do {
				part[counter++] = atoi(split);
			} while ((split = strtok_s(NULL, ".", &rest)) != NULL);
		}
		catch (int /*e*/)
		{
			IpAddress(NULL);
		}
	}

	IpAddress(int _NULL)
	{
		IpAddress((void*)_NULL);
	}

	IpAddress(void* _NULL)
	{
		for (int i = 0; i < 4; ++i)
			part[i] = 0;
	}

	bool operator==(const IpAddress& other)
	{
		for (int i = 0; i < 4; ++i)
			if (part[i] != other.part[i])
				return false;

		return true;
	}

	bool operator==(void* _NULL)
	{
		return isNull();
	}

	bool operator!=(void* _NULL)
	{
		return !isNull();
	}

	bool isSameDomain(const IpAddress& other)
	{
		if (part[0] != other.part[0] || part[1] != other.part[1])
			return false;

		if (abs((int)part[2] - (int)other.part[2]) > 1)
			return false;

		return true;
	}

	bool isNull()
	{
		for (int i = 0; i < 4; ++i)
			if (part[i] != 0)
				return false;

		return true;
	}

	std::string getString()
	{
		ostringstream ss;
		for (int i = 0; i < 4; ++i)
		{
			if (i)
				ss << ".";
			ss << part[i];
		}

		return ss.str();
	}

private:
	int part[4];
};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
const char* getMAC(IpAddress clientIp, IpAddress lookup_ip);
const char* handleMessage(std::string message, sockaddr_in clientInfo);
bool sendMessage(SOCKET client, const char* message);

int main(int argc, char *argv[])
{
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA data;
	sockaddr_in sockname;
	sockaddr_in clientInfo;
	SOCKET mainSocket;
	std::string text;
	char buf[BUFSIZE];
	size_t size;
	int addrlen;
	int port = 3691;

	if (WSAStartup(wVersionRequested, &data) != 0)
	{
		cout << "E: Couldn't initialize sockets." << endl;
		return -1;
	}

	if ((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cerr << "E: Couldn't create socket." << endl;
		WSACleanup();
		return -1;
	}

	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(port);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind(mainSocket, (sockaddr *)&sockname, sizeof(sockname)) == SOCKET_ERROR)
	{
		cerr << "E: Couldn't bind socket." << endl;
		WSACleanup();
		return -1;
	}

	if (listen(mainSocket, 10) == SOCKET_ERROR)
	{
		cerr << "E: Couldn't create listining queue." << endl;
		WSACleanup();
		return -1;
	}

	do
	{
		addrlen = sizeof(clientInfo);

		SOCKET client = accept(mainSocket, (sockaddr*)&clientInfo, &addrlen);
		int totalSize = 0;
		if (client == INVALID_SOCKET)
		{
			cerr << "E: Couldn't accept connection." << endl;
			WSACleanup();
			return -1;
		}

		text.clear();
		do
		{
			size = recv(client, buf, BUFSIZE - 1, 0);
			if (size > 0)
			{
				char *start = buf;
				char *end = NULL;
				while (end = (char*)memchr(start, '\n', size))
				{
					text.append(start, end - start);
					const char* response = handleMessage(text, clientInfo);
					if (response)
						sendMessage(client, response);

					text.clear();
					size -= end - start;
					start = end + 1;
				}

				if (size > 0)
					text.append(start, size);
			}
		} while (text.empty());

		closesocket(client);
	} while (true);

	cout << "Ending" << endl;
	closesocket(mainSocket);
	WSACleanup();
	return 0;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

const char* getMAC(IpAddress clientIp = NULL, IpAddress serverIp = NULL)
{
	IP_ADAPTER_INFO* adapterInfo;
	DWORD dwBufLen = sizeof(adapterInfo);
	char *mac_addr = "";

	adapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (adapterInfo == NULL)
	{
		cout << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << endl;
	}

	if (GetAdaptersInfo(adapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(adapterInfo);
		adapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (adapterInfo == NULL)
		{
			cout << "E: Allocating memory needed to call GetAdaptersinfo went wrong." << endl;
		}
	}

	if (GetAdaptersInfo(adapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
		PIP_ADAPTER_INFO candidate = NULL;

		do {
			IpAddress ipAddress = pAdapterInfo->IpAddressList.IpAddress.String;
			if (ipAddress == NULL)
				continue;

			if (serverIp != NULL)
			{
				if (ipAddress == serverIp)
				{
					candidate = pAdapterInfo;
					break;
				}
			}
			else
			{
				if (candidate)
				{
					if (clientIp != NULL)
					{
						if (clientIp.isSameDomain(ipAddress))
						{
							candidate = pAdapterInfo;
							break;
						}
					}
				}
				else
				{
					candidate = pAdapterInfo;
					if (clientIp == NULL)
						break;
				}
			}

		} while (pAdapterInfo = pAdapterInfo->Next);

		if (candidate)
		{
			mac_addr = (char*)malloc(17);
			_snprintf_s(mac_addr, _TRUNCATE, 17, "%02X:%02X:%02X:%02X:%02X:%02X",
				candidate->Address[0], candidate->Address[1],
				candidate->Address[2], candidate->Address[3],
				candidate->Address[4], candidate->Address[5]);

			//std::string ipAddressString = IpAddress(pAdapterInfo->IpAddressList.IpAddress.String).getString();
		}
	}

	free(adapterInfo);
	return mac_addr;
}

const char* handleMessage(std::string message, sockaddr_in clientInfo)
{
	char* rest = NULL;
	std::string command = strtok_s((char*)message.c_str(), " ", &rest);

	if (command == "TURN_OFF")
	{
		system(COMMAND_TURN_OFF);
		return "OK";
	}
	else if (command == "RESTART")
	{
		system(COMMAND_RESTART);
		return "OK";
	}
	else if (command == "LOCK")
	{
		system(COMMAND_LOCK);
		return "OK";
	}
	else if (command == "SLEEP")
	{
		system(COMMAND_SLEEP);
		return "OK";
	}
	else if (command == "GET_MAC")
	{
		IpAddress serverIp = rest ? IpAddress(rest) : NULL;
		IpAddress clientIp = inet_ntoa((in_addr)clientInfo.sin_addr);
		return getMAC(clientIp, serverIp);
	}

	return NULL;
}

bool sendMessage(SOCKET client, const char* message)
{
	size_t size;
	if ((size = send(client, message, strlen(message), 0)) == SOCKET_ERROR)
	{
		cerr << "Could not send data" << endl;
		WSACleanup();
		return false;
	}

	return true;
}

