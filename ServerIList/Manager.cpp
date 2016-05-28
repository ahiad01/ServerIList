#include "Manager.h"

Manager::Manager() 
{
	string s;
	database = new DataBase();
	server = new Server();
	users = new Users(database);
	groups = new PublicGroup(database);
	lists = new PublicLists(database);
	packets = new UsersPackets(database);
	restore = new Restore(database);
	m_threadRunServer = std::thread(&Manager::runServer, this);
	threads = new thread*[THREADS_MAX]{0};

	this->threads_handler();
	//std::thread t(&Manager::threads_handler, this);

}


Manager::~Manager()
{
	m_threadRunServer.join();
	delete users;
	delete groups;
	delete packets;
	delete lists;
	delete restore;
	delete database;
	delete server;
	delete threads;
}

void Manager::threads_handler()
{

	Message msg;
	int index = THREADS_MAX;
	std::unique_lock<mutex> ul(m);

	while (true)
	{
		msg = server->getLastPacket(); //wait until there is a packet
		index = this->getFreeThreadIndex();
		while (index == THREADS_MAX)
		{
			cv.wait(ul);
		}
		threads[index] = new std::thread(&Manager::message_handler, this, msg, index); //exec request
		threads[index]->join();
		delete threads[index];
	}
}

void Manager::message_handler(Message msg, int index)
{
	string ERROR_MESSAGE = "@500|";
	int sock;

	for (int i = 0; i < server->disconnected_sockets.size(); i)
	{
		sock = server->disconnected_sockets.front();
		if (users->isLogin(sock))
			users->Logout(sock);
		server->disconnected_sockets.pop();
	}
	try{
		code_handler(msg);
	}
	catch (Exception& ex)
	{
		server->sendMessage(msg.getSocket(), ERROR_MESSAGE + ex.what() + "||");

	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		server->sendMessage(msg.getSocket(), ERROR_MESSAGE + "Unknown Error||");
	}
	this->threads[index] = NULL;
	cv.notify_one();
}

void Manager::code_handler(Message msg)
{
	Parser p = Parser(msg.getMessage()); //Parsr the message and check if legal
	int sock = msg.getSocket();
	int code = p.getMsgCode();
	vector<string> args = p.getArguments();
	vector<vector<string>> data;
	string res,receive;
	string sql;

	if (this->users->isLogin(sock) == false && code != EN_REGISTER && code != EN_LOGIN)
		throw UserIsOfflineException();

	string username = this->users->getUsername(sock);
	string admin = username;  //the same -- for easy understanding




	switch (code)
	{
	case EN_REGISTER:
		if (args.size() != 2)
			throw WrongArgumentsNumberException();
		this->users->Register(args[0], args[1]);
		receive = makeReceiveMessgae(PGM_SCC_REGISTER, "");
		server->sendMessage(sock, receive);
		//this->server->sendMessage(sock, receive);
		break;

	case EN_LOGIN:
		if (args.size() != 2)
			throw WrongArgumentsNumberException();
		this->users->Login(sock, args[0], args[1]);
		//data = this->groups->getAllGroupsOfUser(args[0]);
		receive = makeReceiveMessgae(PGM_SCC_LOGIN, "");
		this->trySend(args[0], receive);
		//Send All Messages sent to him when he was not connected.

		//execute it when we will build database on the client!!!
		//data = this->packets->getAllUserPackets(args[0]);
		//for (int col = 0; col < data.size(); col++)
		//	this->trySend(args[0], data[col][0]); //each message
		//	
		//this->packets->deleteAllUserPackets(args[0]);
		break;

	case EN_LOGOUT:
		if (args.size() != 0)
			throw WrongArgumentsNumberException();
		this->users->Logout(sock);
		//this->server->disconnect(sock);
		//TODO -- make receive message
		//this->server->sendMessage(sock, receive);
		break;

	case GR_CREATE_GROUP:
		if (args.size() != 1)
			throw WrongArgumentsNumberException();
		res = this->groups->createGroup(args[0], admin);
		receive = makeReceiveMessgae(PGM_SCC_ACTE_CREATED, res);
		this->server->sendMessage(sock, receive);
		break;

	case GR_GROUP_LIST:
		if (args.size() != 0)
			throw WrongArgumentsNumberException();
		data = this->groups->getAllGroupsOfUser(username);
		receive = makeReceiveMessgae(PGM_GET_GROUPS_LIST, data);
		this->server->sendMessage(sock, receive);
		break;

	case GR_INVITE_USER:
		if (args.size() != 3)
			throw WrongArgumentsNumberException();
		if (this->users->isExist(args[1]) == false) //try to add user does not exist in the database
			throw WrongUsernameException();
		this->groups->addUser(username, args[0], args[1], args[2]);

		args.insert(args.begin() + 1, this->groups->getGroupName(args[0])); //insert group name in args[1]

	
		receive = makeReceiveMessgae(PGM_CTR_NEW_USER, args);

		data = this->groups->getAllUsersInGroup(args[0]);
		this->sendToAll(data, receive);

		/*Send to the new user the current users in the group*/
		for (int col = 0; col < data.size(); col++)
		{
			if (args[2] == data[col][1])
				continue;
			this->trySend(args[2], makeReceiveMessgae(PGM_CTR_NEW_USER,data[col]));
		}

		/*Send to the new user the current list*/
		data = this->lists->getAllItemsInGroup(args[0]);
		for (int col = 0; col < data.size(); col++)
		{
			receive = makeReceiveMessgae(LST_SCC_ADD_ITEM, data[col]);
			trySend(args[2], receive);
		}

		break;

	case GR_REMOVE_USER:
		if (args.size() != 2)
			throw WrongArgumentsNumberException();
		if (this->users->isExist(args[1]) == false) //try to remove user does not exist in the database
			throw WrongUsernameException();
		this->groups->removeUser(username, args[0], args[1]);
		receive = makeReceiveMessgae(PGM_CTR_USER_LEAVE, args);
		this->sendToAll(this->groups->getAllUsersInGroup(args[0]), receive);
		this->trySend(args[0], receive);
		break;

	case GR_SET_STATUS:
		if (args.size() != 3)
			throw WrongArgumentsNumberException();
		if (this->users->isExist(args[1]) == false) //try to set status of user does not exist in the database
			throw WrongUsernameException();
		this->groups->setUserStatus(username, args[0], args[1], args[2]);
		receive = makeReceiveMessgae(PGM_CTR_STATUS_CHANGED, args);
		this->sendToAll(this->groups->getAllUsersInGroup(args[0]), receive);
		break;

	case GR_CLOSE_GROUP:
		if (args.size() != 1)
			throw WrongArgumentsNumberException();
		data = this->groups->getAllUsersInGroup(args[0]);
		this->groups->removeGroup(username, args[0]);
		receive = makeReceiveMessgae(PGM_CTR_GROUP_CLOSED, args);
		this->sendToAll(data, receive);
		break;

	/*case GR_Exit_group -- TODO ------- : */

	case LST_ADD_ITEM:
		if (args.size() != 4)
			throw WrongArgumentsNumberException();
		this->lists->addItem(username, args[0], args[1], args[2], args[3]);
		receive = makeReceiveMessgae(LST_SCC_ADD_ITEM, args);
		this->sendToAll(this->groups->getAllUsersInGroup(args[0]), receive);
		break;

	case LST_REMOVE_ITEM:
		if (args.size() != 2)
			throw WrongArgumentsNumberException();
		this->lists->removeItem(username, args[0], args[1]);
		receive = makeReceiveMessgae(LST_SCC_REMOVE_ITEM, args);
		this->sendToAll(this->groups->getAllUsersInGroup(args[0]), receive);
		break;

	case LST_SET_ITEM:
		if (args.size() != 5)
			throw WrongArgumentsNumberException();
		this->lists->updateItem(username, args[0], args[1], args[2], args[3], args[4]);
		receive = makeReceiveMessgae(LST_SCC_SET_ITEM, args);
		this->sendToAll(this->groups->getAllUsersInGroup(args[0]), receive);
		break;

	case RE_RESTORE:
		if (args.size() != 0)
			throw WrongArgumentsNumberException();
		receive = restore->buildRestoreMessage(username);
		this->trySend(username, receive);
		break;

	}
}


string Manager::makeReceiveMessgae(int msgCode, vector<vector<string>> args)
{
	string code = to_string(msgCode);
	string receive = "@" + code + "|";
	for (int col = 0; col < args.size(); col++)
	{
		for (int row = 0; row < args[0].size(); row++)
		{
			if (row > 0)
				receive += ",";
			receive = receive + args[col][row];
		}
		receive += "|";
	}
	receive += "|";
	return receive;
}

string Manager::makeReceiveMessgae(int msgCode, vector<string> args)
{
	string code = to_string(msgCode);
	string receive = "@" + code + "|";
	for (int col = 0; col < args.size(); col++)
	{
		receive += args[col] + "|";
	}
	receive += "|";
	return receive;
}

string Manager::makeReceiveMessgae(int msgCode, string args)
{
	string code = to_string(msgCode);
	string receive = "@" + code + "|" + args + "||";
	return receive;
}

void Manager::trySend(string username, string message)
{
	int sock;
	string sql;


	if (users->isLogin(username)) //if the username is login
	{
		sock = this->users->getSock(username);
		//cout << "Send To " + username + "\tdata:" + message << endl;
		server->sendMessage(sock, message);
	}
	else
	{
		sql = "INSERT INTO UsersPackets (username, message) VALUES('" + username + "','" + message + "');";
		database->query(sql);
		if(this->server->getPrintFlag()) cout << "Saved Message For " + username + "\tdata:" + message << endl;
	}
}

void Manager::sendToAll(vector<vector<string>> users, string message)
{
	for (int col = 0; col < users.size(); col++)
		trySend(users[col][1], message); //index 1 = username
}

void Manager::runServer()
{
	server->run();
}

int Manager::getFreeThreadIndex()
{
	int i;
	for (i = 0; i < THREADS_MAX; i++)
	{
		if (threads[i] == NULL)
			break;
	}
	return i;
}

Server* Manager::getServer()
{
	return this->server;
}