#include "Message.h"

Message::Message(std::string message, SOCKET socket)
{
	m_message = message;
	m_socket = socket;
}


Message::~Message()
{
}

SOCKET Message::getSocket()
{
	return m_socket;
}


std::string Message::getMessage()
{
	return m_message;
}

void Message::setMessage(std::string msg)
{
	this->m_message = msg;
}

void Message::setSocket(int sock)
{
	this->m_socket = sock;
}