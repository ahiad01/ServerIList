#pragma once
#include "Database.h"
#include <vector>
#include <string>

using namespace std;

class UsersPackets
{
public:
	UsersPackets(DataBase* database);
	~UsersPackets();

	vector<vector<string>> getAllUserPackets(string username); //get all saved packets
	void deleteAllUserPackets(string username); //delete all saved packets
	void addUserPacket(string username, string packet); //save a packet

private:
	DataBase& m_database;
};

