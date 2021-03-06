// FTPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>


using namespace boost::asio;
using boost::system::error_code;
using ip::tcp;
using std::string;
using std::cout;
using std::cerr;
using std::endl;


string read(tcp::socket& socket)
{
	streambuf buffer;
	string data;
	
	read_until(socket, buffer, "\n");
	
	data = buffer_cast <const char*> (buffer.data());

	return data;
}

void send(tcp::socket& socket, string message)
{
	error_code errorCode;
	const string msg = message + "\n";

	write(socket, buffer(msg), errorCode);
	
	if (errorCode)
	{
		cerr << "Blad wysylania: " << errorCode.message() << endl;
	}
}

int main()
{
	int port = 6969;
	string message;
	string msg;
	io_service io_service;
	error_code errorCode;
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

	cout << "Serwer uruchomiony na porcie: " << port << endl;

	tcp::socket socket(io_service);

	acceptor.accept(socket, errorCode);

	if (errorCode)
	{
		cerr << "Acceptor error: " << errorCode.message() << endl;
	}
	else
	{
		cout << "Klient polaczyl sie!" << endl;
	}

	while (true)
	{
		message = "Serwer: " + read(socket);
		cout << message << endl;
		send(socket, message);
	}
	
	system("pause");

    return 0;
}

