#pragma once
#include "Message.h"
#include "Database.h"
#include "Parser.h"
#include "Users.h"
#include "Server.h"
#include "PublicLists.h"
#include "PublicGroup.h"
#include "UsersPackets.h"
#include "Restore.h"
#include <thread>
#include <string>
#include <condition_variable>


#define THREADS_MAX 10

using namespace std;

class Manager
{
public:
	Manager();
	~Manager();
	void threads_handler();
	void message_handler(Message msg, int index);
	void code_handler(Message msg);
	void runServer();

	Server* getServer();

	int getFreeThreadIndex();

private:
	thread** threads;
	DataBase* database;
	Users* users;
	PublicGroup* groups;
	PublicLists* lists;
	UsersPackets* packets;
	Server* server;
	Restore* restore;
	thread m_threadRunServer;

	mutex m;
	condition_variable cv;

	void CreateDatabaseTables();
	string makeReceiveMessgae(int msgCode, vector<vector<string>> args);
	string makeReceiveMessgae(int msgCode, vector<string> args);
	string makeReceiveMessgae(int msgCode, string args);
	
	void trySend(string username, string message);

	void sendToAll(vector<vector<string>> users, string message);

	string buildRestoreMessage(string username);


};

