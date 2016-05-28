#include "Users.h"


Users::Users(DataBase* db) : database(ref(*db))
{
	this->online_users = map<int, string>();
	this->online_users_opposite = map<string, int>();
}


Users::~Users()
{
}

void Users::Login(int sock, string username, string password) throw()
{
	std::map<int, string>::iterator it1;
	std::map<string, int>::iterator it2;
	vector<vector<string>> data;

	bool is_valid = is_valid_username(username.length()) == true && is_valid_password(password.length()) == true;
	if (!is_valid)
		throw InvalidUsernameOrPasswordException();
	string sql = "select * from Users where username = '" + username + "' and password = '" + password + "';";
	if (database.query(sql) == DataBase::Empty) //not found
		throw WrongUsernameOrPasswordException();

	if (online_users.size()){
		if ((it1 = online_users.find(sock)) != online_users.end()) //The user is already logged in from this socket !!!
		{
			//throw UserNotAvilableException();
		}
		else if ((it2 = online_users_opposite.find(username)) != online_users_opposite.end()) //The user is already logged in from this username !!!
		{
			
			throw UserNotAvilableException();
		}
	}
	//on success
	add(sock, username);
	
}


void Users::Register(string username, string password) throw()
{
	string sql;
	bool is_valid = is_valid_username(username.length()) == true && is_valid_password(password.length()) == true;
	if (!is_valid)
		throw InvalidUsernameOrPasswordException();

	sql = "select * from Users where username = '" + username + "';"; //selct query
	if (database.query(sql) != DataBase::Empty) //user not avilable
		throw UserNotAvilableException();

	sql = "INSERT INTO Users VALUES('" + username + "','" + password + "');"; //insert query
	database.query(sql);
}

void Users::Logout(int sock)
{
	remove(sock, getUsername(sock));
}

void Users::Logout(string username)
{
	remove(getSock(username), username);
}

string Users::getUsername(int sock)
{
	return this->online_users[sock];
}

int Users::getSock(string username)
{
	return this->online_users_opposite[username];
}

void Users::add(int sock, string username)
{
	this->online_users[sock] = username;
	this->online_users_opposite[username] = sock;
}

void Users::remove(int sock, string username)
{
	this->online_users.erase(sock);
	this->online_users_opposite.erase(username);
}

bool Users::is_valid_username(int len)
{
	if (len >= MIN_USERNAME_LENGTH && len <= MAX_USERNAME_LENGTH)
		return true;
	return false;
}

bool Users::is_valid_password(int len)
{
	if (len >= MIN_PASSWORD_LENGTH && len <= MAX_PASSWORD_LENGTH)
		return true;
	return false;
}

bool Users::isExist(string username)
{
	string sql = "SELECT * FROM Users WHERE username ='" + username + "';";
	if (database.query(sql) == DataBase::Empty)
		return false;
	return true;
}

bool Users::isLogin(int sock)
{
	if (this->online_users.find(sock) == this->online_users.end())
		return false;
	return true;
}

bool Users::isLogin(string username)
{
	if (this->online_users_opposite.find(username) == this->online_users_opposite.end())
		return false;
	return true;
}