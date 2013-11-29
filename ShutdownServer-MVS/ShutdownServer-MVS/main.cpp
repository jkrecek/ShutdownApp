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
	cout << "Handling: " << message;
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
		cerr << "Problém s odesláním dat" << endl;
		WSACleanup();
		return false;
	}
	cout << "Odesláno: " << size << endl;
	return true;
}

int main(int argc, char *argv[]) { 
	WORD wVersionRequested = MAKEWORD(1, 1); // Èíslo verze
	WSADATA data;           // Struktura s info. o knihovnì;    
	std::string text;       // Pøijímaný text
	sockaddr_in sockname;   // "Jméno" soketu a èíslo portu
	sockaddr_in clientInfo; // Klient, který se pøipojil 
	SOCKET mainSocket;      // Soket
	char buf[BUFSIZE];      // Pøijímací buffer
	size_t size;            // Poèet pøijatých a odeslaných bytù
	int addrlen;            // Velikost adresy vzdáleného poèítaèe
	int count = 0;          // Poèet pøipojení
	int port = 3691;

	if (WSAStartup(wVersionRequested, &data) != 0)
	{
		cout << "Nepodaøilo se inicializovat sokety" << endl;
		return -1;
	}

	if ((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cerr << "Nelze vytvoøit soket" << endl;
		WSACleanup();
		return -1;
	}
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(port);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind(mainSocket, (sockaddr *)&sockname, sizeof(sockname)) == SOCKET_ERROR)
	{
		cerr << "Problém s pojmenováním soketu." << endl;
		WSACleanup();
		return -1;
	}

	if (listen(mainSocket, 10) == SOCKET_ERROR)
	{
		cerr << "Problém s vytvoøením fronty" << endl;
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
			cerr << "Problém s pøijetím spojeni" << endl;
			WSACleanup();
			return -1;
		}

		cout << "Nìkdo se pøipojil z adresy: " << inet_ntoa((in_addr)clientInfo.sin_addr) << endl;

		text = "";
		
		do {
			cout << "Reading" << endl;
			size = recv(client, buf, BUFSIZE - 1, 0);
			if (size > 0) {
				text.append(buf, size);
			}
		} while (size >= (BUFSIZE - 1));

		std::vector<std::string> messages = split(text, '\n');
		for (std::vector<std::string>::iterator itr = messages.begin(); itr != messages.end(); ++itr) {
			std::string message = *itr;
			const char* response = handleMessage(message);
			if (response) {
				sendMessage(client, response);
			}
		}

		closesocket(client);
	} while (++count != 3);
	cout << "Konèím" << endl;
	closesocket(mainSocket);
	WSACleanup();
	return 0;
}
