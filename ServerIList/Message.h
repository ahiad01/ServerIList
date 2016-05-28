#pragma once
#include <string>
#include <WinSock2.h>
#include <vector>

class Message
{
public:
	Message(){};//default constractor
	Message(std::string message, SOCKET socket);//constractor
	~Message();//destractor
	std::string getMessage();//returns the message
	SOCKET getSocket();//returns the socket
	void setMessage(std::string msg);//sets the message
	void setSocket(int sock);//sets the socket

private:
	std::string m_message;//message
	SOCKET m_socket;//socket
};