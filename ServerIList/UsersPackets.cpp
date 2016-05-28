#include "UsersPackets.h"


UsersPackets::UsersPackets(DataBase* database) : m_database(ref(*database))
{}

UsersPackets::~UsersPackets()
{
}


vector<vector<string>> UsersPackets::getAllUserPackets(string username)
{
	string sql = "SELECT message FROM UsersPackets WHERE username ='" + username + "';";
	return m_database.query(sql);
}

void UsersPackets::deleteAllUserPackets(string username)
{
	string sql = "DELETE FROM UsersPackets WHERE username='" + username + "';";
	m_database.query(sql);
}

void UsersPackets::addUserPacket(string username, string packet)
{
	string sql = "INSERT INTO UsersPackets (username, message) VALUES('" + username + "','" + packet + "');";
	m_database.query(sql);
}