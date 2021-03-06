// FTPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "FTPserver.h"
#include <fstream>
#include <exception>


using namespace boost::asio;
using boost::system::error_code;
using ip::tcp;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::filebuf;

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
	const char* message = "empty";
	bytesRecieved = read_until(socket, buf, "\n", errorCode);

	if (errorCode && errorCode != boost::asio::error::eof)
	{
		cout << "Receive failed: " << errorCode.message() << endl;
	}
	else
	{

		message = boost::asio::buffer_cast<const char*>(buf.data());

		//cout << message << endl;

	}
	buf.consume(buf.size());

	return message;
}

string listFiles()
{
	string directoryContent;
	boost::filesystem::path p = boost::filesystem::current_path();
	//{ "D:\\programowanie\\sieciowe\\files" };
	boost::filesystem::directory_iterator it{ p };

	for (it; it != boost::filesystem::directory_iterator{}; it++)
	{
			directoryContent.append(it->path().filename().string());

		directoryContent.append("\n");
	}

	return directoryContent;
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
		sendMessage("Nie ma takiego pliku!", socket);
	}
	else
	{
		int fileSize;

		fileSize = data.size();
		sendMessage(std::to_string(fileSize), socket);
		boost::asio::write(socket, boost::asio::buffer(data));
		cout << "Wyslano plik: " << fileName << endl;
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
	string message;
	string msg;
	string fileName;
	io_service io_service;
	error_code errorCode;
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
	tcp::socket socket(io_service);

	try {
		cout << "Serwer uruchomiony na porcie: " << port << endl;

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
			sendMessage("1. Wypisz pliki\n2. Wgraj plik\n3. Pobierz plik\n4. Rozlacz", socket);
			switch (stoi(recieveMessage(socket)))
			{
			case 1:
				sendMessage(listFiles(), socket);
				break;
			case 2:
				fileName = recieveMessage(socket);
				fileName.pop_back();
				recieveFile(fileName, socket);
				break;
			case 3:
				sendMessage("Wprowadz pelna nazwe pliku do pobrania: ", socket);
				fileName = recieveMessage(socket);
				fileName.pop_back();
				sendFile(fileName, socket);

				break;
			case 4:
				socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
				socket.close();
			default:
				break;
			}
		}
	}
	catch(std::exception& e)
	{
		cout << e.what();
	}
	
	system("pause");

    return 0;
}
