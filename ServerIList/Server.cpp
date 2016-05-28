#include "Server.h"


Server::Server()
{
	clients = std::vector<SOCKET>();
	this->disconnected_sockets = std::queue<int>();
}

//d'tor - closing the sockets and its' stuff
Server::~Server()
{
	closesocket(_ClientSocket);
	WSACleanup();
}

void Server::run()
{

	_ListenSocket = INVALID_SOCKET;
	_ClientSocket = INVALID_SOCKET;

	_flag = 0;

	_si = sizeof(_clientInfo);

	_result = NULL;

	// Initialize Winsock
	_iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (_iResult != 0)
	{
		std::cout << "WSAStartup failed with error:" << std::endl << _iResult;
		system("PAUSE");
	}
	else
	{
		ZeroMemory(&(_hints), sizeof(_hints));
		_hints.ai_family = AF_INET;
		_hints.ai_socktype = SOCK_STREAM;
		_hints.ai_protocol = IPPROTO_TCP;
		_hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		_iResult = getaddrinfo(NULL, DEFAULT_PORT, &(_hints), &(_result));
		if (_iResult != 0)
		{
			std::cout << "getaddrinfo failed with error: %d" << std::endl << _iResult;
			WSACleanup();
		}
		else
		{
			// Create a SOCKET for connecting to server
			_ListenSocket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);
			if (_ListenSocket == INVALID_SOCKET)
			{
				std::cout << "socket failed with error: %ld" << std::endl << WSAGetLastError();
				system("PAUSE");
				WSACleanup();
			}
			else
			{
				// Setup the TCP listening socket
				bind(_ListenSocket, _result->ai_addr, (int)_result->ai_addrlen);
				if (_iResult == SOCKET_ERROR)
				{
					std::cout << "bind failed with error:" << std::endl << WSAGetLastError();
					system("PAUSE");
					closesocket(_ListenSocket);
					WSACleanup();
				}
				else
				{

					listenFunction();
					std::cout << "The server is listening on port " << DEFAULT_PORT << " !" << std::endl;
					//after the server is listening

					//

					std::thread t1(&Server::acceptFunction, this);
					std::thread t2(&Server::recvFunction, this);

					t1.join();
					t2.join();
				}
			}
		}
	}
}

void Server::listenFunction()
{
	_iResult = listen(_ListenSocket, SOMAXCONN);
	if (_iResult == SOCKET_ERROR)
	{
		printf("listen failed with error : %d\n", WSAGetLastError());
		system("PAUSE");
		closesocket(_ListenSocket);
		WSACleanup();
	}
}

void Server::acceptFunction()
{
	while (true)
	{
		_ClientSocket = accept(_ListenSocket, (struct sockaddr*)&(_clientInfo), &(_si)); // Accept a client socket
		if (_ClientSocket == INVALID_SOCKET)
		{
			std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
			closesocket(_ListenSocket);
			WSACleanup();
		}
		else
		{
			clients.push_back(_ClientSocket);
			
			if(printFlag) std::cout << "Accept on Socket: " << _ClientSocket << std::endl;

			cv.notify_one();
		}
	}
}

void Server::recvFunction()
{
	Message msg;
	std::unique_lock<std::mutex> lk(m);
	int index = 0;
	while (true)
	{
		queueMutex.lock();
		if (clients.empty())
		{
			if (printFlag) std::cout << "Wait for a client\n";
			cv.wait(lk);
		}
		if (index >= clients.size()) //running in a Loop
			index = 0;
		if (isReadyToRecv(clients[index], 0.1))
		{
			memset(_buffer, 0, strlen(_buffer)); //clear the buffer
			_iResult = recv(clients[index], _buffer, DEFAULT_BUFLEN, 0);
			if (_iResult == SOCKET_ERROR || _iResult < MIN_LENGTH_MESSAGR)
			{
				disconnect(clients[index]);

			}
			else
			{
				msg.setMessage(std::string(_buffer));
				msg.setSocket(clients[index]);
				messages.push(msg);
				if (printFlag) std::cout << "recv Data From: " << clients[index] << "\tData: " << _buffer << std::endl;
				queueCV.notify_one();
			}
		}
		index++;
		queueMutex.unlock();
	}
}

bool Server::isReadyToRecv(int socket, int interval = 1)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socket, &fds);

	timeval tv;
	tv.tv_sec = interval;
	tv.tv_usec = 0;

	return(select(socket + 1, &fds, 0, 0, &tv) == 1);
}

Message Server::getLastPacket()
{
	Message msg;
	std::unique_lock<std::mutex> lk(m2);

	if (messages.empty())
	{
		queueCV.wait(lk);
	}
	queueMutex.lock();
	msg = messages.front();
	messages.pop();
	queueMutex.unlock();
	return msg;

}

void Server::sendMessage(int socket, std::string message)
{
	if (printFlag) std::cout << "Send Data To: " << socket << "\tData: " + message << std::endl << std::endl;
	if (send(socket, message.c_str(), strlen(message.c_str()), NULL) == SOCKET_ERROR)
		disconnect(socket);

	//std::cout << "sendeing to " << socket << "-" << message << std::endl;
}

void Server::disconnect(int socket)
{
	if (clients.size())
	{
		if (std::find(clients.begin(), clients.end(), socket) != clients.end())
		{
			clients.erase(std::remove(clients.begin(), clients.end(), socket));
			this->disconnected_sockets.push(socket);
			closesocket(socket);
		}
	}
}

void Server::setPrintFlag(bool state)
{
	this->printFlag = state;
}

bool Server::getPrintFlag()
{
	return this->printFlag;
}