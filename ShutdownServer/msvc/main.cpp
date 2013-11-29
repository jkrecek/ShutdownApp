#include <iostream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>


#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 1000

#define COMMAND_TURN_OFF "shutdown /s /f /t 0"
#define COMMAND_RESTART "shutdown /r /f /t 0"
#define COMMAND_LOCK "rundll32.exe user32.dll,LockWorkStation"
#define COMMAND_SLEEP "rundll32 powrprof.dll,SetSuspendState Standby"

using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

const char* handleMessage(std::string message) {
	if (message == "TURN_OFF") {
		system(COMMAND_TURN_OFF);
		return "OK";
	}
	else if (message == "RESTART") {
		system(COMMAND_RESTART);
		return "OK";
	}
	else if (message == "LOCK") {
		system(COMMAND_LOCK);
		return "OK";
	}
	else if (message == "SLEEP") {
		system(COMMAND_SLEEP);
		return "OK";
	}

	return NULL;
}

bool sendMessage(SOCKET client, const char* message) {
	size_t size;
	if ((size = send(client, message, strlen(message), 0)) == SOCKET_ERROR)
	{
		cerr << "Could not send data" << endl;
		WSACleanup();
		return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA data;
	std::string text;
	sockaddr_in sockname;
	sockaddr_in clientInfo;
	SOCKET mainSocket;
	char buf[BUFSIZE];
	size_t size;
	int addrlen;
	int count = 0;
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
		cerr << "E: Couldn't name socket." << endl;
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


		std::string text;
		do {
			size = recv(client, buf, BUFSIZE - 1, 0);
			if (size > 0) {
				char *start = buf;
				char *end = NULL;
				while (end = (char*)memchr(start, '\n', size))
				{
					text.append(start, end - start);
					const char* response = handleMessage(text);
					if (response) {
						sendMessage(client, response);
					}

					text.clear();
					size -= end - start;
					start = end + 1;
				}

				if (size > 0)
					text.append(start, size);
			}
		} while (text.empty());

		closesocket(client);
	} while (++count != 3);
	cout << "Ending" << endl;
	closesocket(mainSocket);
	WSACleanup();
	return 0;
}
