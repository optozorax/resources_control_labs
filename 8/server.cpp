#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <algorithm>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

//-----------------------------------------------------------------------------
class Server
{
public:
	Server(int port);
	~Server();

	int getSocket(void) const;

private:
	int m_socket;

	int createSocket(void) const;
	void bindSocket(int portno) const;
};

//-----------------------------------------------------------------------------
class Client
{
public:
	Client(const Server& server);
	~Client();

	bool isHaveMessage(void);

	std::string getIP(void) const;

	std::string getString(void);
	void sendString(const std::string& msg);

private:
	int m_socket;
	std::string ip;
};

//-----------------------------------------------------------------------------
void error(const char *msg);

//=============================================================================
//=============================================================================
//=============================================================================

//-----------------------------------------------------------------------------
Server::Server(int port) {
	// Получаем сокет
	m_socket = createSocket();

	// Биндим сокет с адресом и портом
	bindSocket(port);

	// Ожидаем подключения и говорим о максимальном количестве подключений
	listen(m_socket, 1);
}

//-----------------------------------------------------------------------------
Server::~Server() {
	closesocket(m_socket);
}

//-----------------------------------------------------------------------------
int Server::getSocket(void) const {
	return m_socket;
}

//-----------------------------------------------------------------------------
int Server::createSocket(void) const {
	int result = socket(AF_INET, SOCK_STREAM, 0);
	if (result < 0) 
		error("ERROR opening socket");
	else
		return result;
}

//-----------------------------------------------------------------------------
void Server::bindSocket(int portno) const {
	sockaddr_in serv_addr;
	memset((char*)(&serv_addr), sizeof(serv_addr), 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(m_socket, (struct sockaddr *)(&serv_addr), sizeof(serv_addr)) < 0)
		error("ERROR on binding");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Client::Client(const Server& server) {
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	m_socket = accept(
		server.getSocket(), 
		(struct sockaddr*)(&cli_addr), 
		&clilen
	);

	ip = std::string(inet_ntoa(cli_addr.sin_addr));

	if (m_socket < 0) 
		error("ERROR on accept");
}

//-----------------------------------------------------------------------------
Client::~Client() {
	closesocket(m_socket);
}

//-----------------------------------------------------------------------------
bool Client::isHaveMessage(void) {
	u_long n;
	int rt = ioctlsocket(m_socket, FIONREAD, &n);
	return n > 0;
}

//-----------------------------------------------------------------------------
std::string Client::getIP(void) const {
	return ip;
}

//-----------------------------------------------------------------------------
std::string Client::getString(void) {
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
void Client::sendString(const std::string& msg) {
	int n = send(m_socket, msg.c_str(), msg.size(), 0);
	if (n < 0)
		error("ERROR writing to socket");
}

//-----------------------------------------------------------------------------
void error(const char *msg) {
	std::cerr << msg << std::endl;
	system("pause");
	exit(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char* argv[]) {	
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	std::string sum = "random";
	if (argc > 1) sum = argv[1];

	// Создаем сервер
	Server server(2006);
	Client client(server);

	std::cout << "Client connected." << std::endl;

	while (true) {
		if (client.isHaveMessage()) {
			auto str = client.getString();

			std::cout << "Received: \"" << str << "\"" << std::endl;

			str = "Performing: " + str + ", with result: " + sum;

			std::cout << "Send to client: \"" << str << "\"" << std::endl;

			client.sendString(str);
		}
	}
}