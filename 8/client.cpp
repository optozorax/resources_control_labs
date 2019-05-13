#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <locale>
#include <codecvt>
#include <map>
#include <thread>
#include <iostream>
#include <string>
#include <atomic> 
#include <functional>

//-----------------------------------------------------------------------------
class Server
{
public:
	Server(std::string IP, int port);
	~Server();

	std::string getString(void);
	void sendString(const std::string& msg);

	bool isHaveString(void) const;
private:
	SOCKET m_socket;
};

//-----------------------------------------------------------------------------
void error(const char *msg);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
Server::Server(std::string IP, int port) {
	WSADATA wsaData;
	sockaddr_in ServerAddr;

	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	// Создаем сокет
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket < 0)
		error("ERROR opening socket");

	// Подключаемся к серверу
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(IP.c_str());
	ServerAddr.sin_port = htons(port);
	int err = connect(m_socket, (sockaddr *)(&ServerAddr), sizeof(ServerAddr));

	if (err == SOCKET_ERROR)
		error("ERROR on connecting");
}

//-----------------------------------------------------------------------------
Server::~Server() {
	closesocket(m_socket);
	WSACleanup();
}

//-----------------------------------------------------------------------------
std::string Server::getString(void) {
	const int bufferSize = 256;
	const int bufferSize2 = bufferSize+1;
	static char buffer[bufferSize];
	static char buffer2[bufferSize2];

	std::string result;

	bool isShouldRead = true;
	while (isShouldRead) {
		int n = recv(m_socket, buffer, bufferSize-1, MSG_PEEK);
		buffer[n] = 0;
		if (n < 0)
			error("ERROR reading from socket");
		result += buffer;

		// Костыль для определения, всё ли мы считали, или нет
		int n2 = recv(m_socket, buffer2, bufferSize2-1, MSG_PEEK);
		isShouldRead = n != n2;
		n = recv(m_socket, buffer, bufferSize-1, 0);
	}

	return result;
}

//-----------------------------------------------------------------------------
bool Server::isHaveString(void) const {
	unsigned long l;
	auto returned = ioctlsocket(m_socket, FIONREAD, &l);
	return l > 0;
}

//-----------------------------------------------------------------------------
void Server::sendString(const std::string& msg) {
	int n = send(m_socket, msg.c_str(), msg.size(), 0);
	if (n < 0)
		error("ERROR writing to socket");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void error(const char *msg) {
	std::cerr << msg << std::endl;
	exit(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	//Server server("217.71.129.139", 4010); // Глобальная сеть
	Server server("127.0.0.1", 2006); // Локальная сеть

	std::cout << "Connected to server" << std::endl;

	while (true) {
		Sleep(rand() % 200);

		if (server.isHaveString())
			std::cout << "Received: \"" << server.getString() << "\"" << std::endl;

		if (rand() % 5 == 0) {
			int task = rand() % 100;
			std::cout << "Send to server: \"" << task << "\"" << std::endl;
			server.sendString(std::to_string(task));
		}
	}	
}