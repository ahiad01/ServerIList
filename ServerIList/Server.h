#ifndef SERVER_H
#define SERVER_H

#pragma comment(lib, "wsock32.lib")
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <string.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "Message.h"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8000"
#define IP1 "192.168.1.14"
#define PASSWORD "2708"
#define BUFFER_LENGTH 1024
#define MIN_LENGTH_MESSAGR 4 //@x||



class Server
{
private:
	WSADATA wsaData; //variables for the server to work

	int _iResult,
		_send_result,
		_recv_result,
		_recvbuflen,
		_flag,
		_pass,
		_si;

	SOCKET _ListenSocket;
	SOCKET _ClientSocket;

	struct sockaddr_in _clientInfo;

	struct addrinfo* _result;

	struct addrinfo _hints;

	std::string _send_enterance,
		_recvbuf,
		_recvquery;

	char _buffer[DEFAULT_BUFLEN]; //doesnt work with string

	std::vector<SOCKET> clients;
	std::queue<Message> messages;

	std::mutex m, m2;
	std::mutex queueMutex;
	std::condition_variable cv;
	std::condition_variable queueCV;


	bool printFlag = true;

	void listenFunction();
	void acceptFunction();
	void recvFunction();
	bool isReadyToRecv(int socket, int interval);

public:
	Server();
	~Server();

	void run();
	void sendMessage(int socket, std::string message);
	Message getLastPacket();
	void disconnect(int socket);

	int getClients(){ return clients.size(); }
	std::queue<int> disconnected_sockets;

	void setPrintFlag(bool state);
	bool getPrintFlag();
};
#endif