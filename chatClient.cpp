// chatClient.cpp : Defines the entry point for the console application
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#define PORT 1414
#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(version, &wsaData))
	{
		cerr << "Winsock init error." << endl;
		exit(1);
	}

	SOCKET clientSocket;
	char buffer[1024];
	sockaddr_in address;
	string ipAddress = "127.0.0.1";
	string input;
	int bytesRecieved;

	if ((clientSocket = socket(PF_INET, SOCK_STREAM, 0) == INVALID_SOCKET)) 
	{
		cerr << "Socket error: " << WSAGetLastError() << endl;
		exit(1);
	}


	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr);
	memset(&(address.sin_zero), '\0', 8);

	if (connect(clientSocket, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		cerr << "Connection to server error: " << WSAGetLastError() << endl;
		//closesocket(clientSocket);
		//exit(1);
	}

	do
	{
		cout << "> ";
		getline(cin, input);
		if (send(clientSocket, input.c_str(), input.size() + 1, 0) == SOCKET_ERROR)
		{
			cerr << "Sending error: " << WSAGetLastError() << endl;
			//exit(1);
		}
		else
		{

			if (bytesRecieved = recv(clientSocket, buffer, 1024, 0))
			{
				cout << "SERVER> " << string(buffer, 0, bytesRecieved);
			}
		}
	} while (input.size() > 0);

	closesocket(clientSocket);
	WSACleanup();

	system("pause");
    return 0;
}

