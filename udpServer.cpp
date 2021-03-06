// udpServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << "Należy podać z lini komend nazwe grupy i port." << endl;
	}

	string group(argv[1]);
	int port = atoi(argv[2]);

	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOk = WSAStartup(version, &data);
	if (wsOk == 0)
	{
		cerr << "Winsock startup problem " << wsOk <<endl;
	}

	SOCKET sd;
	bool yes = true;
	sockaddr_in address;
	ip_mreq mreq;
	string message;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
	{
		cerr << "Socket error: " << WSAGetLastError() << endl;
		exit(1);
	}

	

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) == SOCKET_ERROR)
	{
		cerr << "Socket setting error: " << WSAGetLastError() << endl;
		exit(1);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if (bind(sd, (sockaddr*)&address, sizeof(address)))
	{
		cerr << "Bind error: " << WSAGetLastError() << endl;
		exit(1);
	}

	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	mreq.imr_multiaddr.s_addr = inet_addr(group);

	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) == SOCKET_ERROR)
	{
		cerr << "Setsockopt error: " << WSAGetLastError();
		exit(1);
	}

	while (true)
	{
		cout << ">";
		getline(cin, message);

		if(sendto(sd, message, ))
		else
		{
			recvfrom
		}
	}

	system("pause");
    return 0;
}

