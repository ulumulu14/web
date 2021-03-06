// FTPClient.cpp : Defines the entry point for the console application.
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
using std::cin;
using std::cerr;
using std::endl;
using std::istream;
using std::ofstream;
using std::ifstream;

void sendMessage(string message, tcp::socket& socket)
{
	error_code errorCode;
	const string msg = message + "\n";

	write(socket, buffer(msg), errorCode);

	if (errorCode)
	{
		cerr << "Blad wysylania: " << errorCode.message() << endl;
	}
}

string recieveMessage(tcp::socket& socket)
{
	int bytesRecieved;
	streambuf buf;
	error_code errorCode;
	const char* message = "none";
	bytesRecieved = read_until(socket, buf, "\n", errorCode);

	if (errorCode && errorCode != boost::asio::error::eof)
	{
		cout << "Receive failed: " << errorCode.message() << endl;
	}
	else
	{
		message = boost::asio::buffer_cast<const char*>(buf.data());
	}
	buf.consume(buf.size());

	return message;
}

string openFile(const string &fileName)
{
	char buffer[1024];
	string reply;
	ifstream file;
	int fileSize;


	file.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if ((file.rdstate() & std::ifstream::failbit) != 0)
		return "Nie ma takiego pliku!";

	fileSize = file.tellg();
	while (file.read(buffer, sizeof(buffer)).gcount() > 0)
		reply.append(buffer, file.gcount());

	return reply;
}

void sendFile(const string& fileName, tcp::socket& socket)
{

	string data = openFile(fileName);

	if (data == "Nie ma takiego pliku!")
	{
		sendMessage(data, socket);
		cout << data << endl;
	}
	else
	{
		int fileSize;

		fileSize = data.size();
		sendMessage(std::to_string(fileSize), socket);
		cout << "Wgrywam na serwer plik: " << fileName << endl;
		boost::asio::write(socket, boost::asio::buffer(data));
	}
	
}

void recieveFile(const string& fileName, tcp::socket& socket)
{
	char buf[1000000];
	streambuf buffer;
	istream stream(&buffer);
	string message;
	int bytesRecieved;
	int fileSize;
	ofstream ofs;
	error_code errorCode;
	
	message = recieveMessage(socket);

	if (message == "Nie ma takiego pliku!\n")
	{
		cout << message << endl;
		return;
	}
	else
	{
		fileSize = stoi(message);
		cout << "Pobieram plik: " << fileName << " Rozmiar: " << fileSize << " bajtow" << endl;
		bytesRecieved = boost::asio::read(socket, buffer, boost::asio::transfer_exactly(fileSize), errorCode);
		if (errorCode && errorCode != boost::asio::error::eof)
			cout << "Receive failed: " << errorCode.message() << endl;
		else
			cout << "Pobieranie ukonczone." << endl;

		stream.read(buf, fileSize);
		ofs.open(fileName, std::ios_base::binary);
		ofs.write(buf, fileSize);
		buffer.consume(buffer.size());
	}
}

int main()
{
	int port = 6969;
	int bytesRecieved;
	const char* data;
	string choice;
	string message;
	string fileName;
	io_service io_service;
	error_code errorCode;
	tcp::socket socket(io_service);
	streambuf buf;

	socket.connect(tcp::endpoint(ip::address::from_string("127.0.0.1"), port), errorCode);

	if (errorCode)
	{
		cerr << "Connection error: " << errorCode.message() << endl;
	}
	else
	{
		cout << "Nawiazano polaczenie z serwerem!" << endl;
	}

	while (true)
	{
		cout << recieveMessage(socket);
		cin >> choice;
		message = choice + "\n";
		sendMessage(message, socket);

		switch (stoi(choice))
		{
		case 1:
			cout << recieveMessage(socket);
			break;
		case 2:
			cout << "Wprowadz lokalizacje pliku do wgrania na serwer: ";
			cin >> fileName;
			sendMessage(fileName.substr(fileName.find_last_of("/\\") + 1), socket);
			sendFile(fileName, socket);
			break;
		case 3:
			cout << recieveMessage(socket);
			cin >> fileName;
			sendMessage(fileName, socket);
			recieveFile(fileName, socket);
			break;
		case 4:
			socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
			socket.close();
			exit(1);
		default:
			break;
		}
	}

	return 0;
}

