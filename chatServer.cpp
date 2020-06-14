// chatServer.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <string.h>
#include <iostream>
#define PORT 1414

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

	SOCKET serverSocket;
	SOCKET clientSocket;
	int clientSize;
	bool optVal = true;
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		cerr << "Socket error: " << WSAGetLastError() << endl;
		exit(1);
	}

	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &optVal, sizeof(optVal)) == SOCKET_ERROR)
	{
		cerr << "Setsockopt error: " << WSAGetLastError() << endl;
		exit(1);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(serverAddr.sin_zero), '\0', 8);

	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cerr << "Bind error: " << WSAGetLastError() << endl;
		exit(1);
	}

	if (listen(serverSocket, SOMAXCONN))
	{
		cerr << "Listen error: " << WSAGetLastError() << endl;
		exit(1);
	}

	clientSize = sizeof(clientAddr);
	if ((clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize)) == INVALID_SOCKET)
	{
		cerr << "Accept error: " << WSAGetLastError() << endl;
		exit(1);
	}


	system("pause");
    return 0;
}

