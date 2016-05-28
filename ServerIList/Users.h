#pragma once

#ifndef USERNAME_AND_PASSWORD_lENGTHS
#define MAX_USERNAME_LENGTH 20
#define MIN_USERNAME_LENGTH 4
#define MAX_PASSWORD_LENGTH 20
#define MIN_PASSWORD_LENGTH 6

#endif

#include "Exception.h"
#include "Database.h"
#include "Server.h"
#include <map>
#include <string>

using namespace std;

class Users
{
public:
	Users();
	Users(DataBase* db);
	~Users();

	void Login(int sock, string username, string password) throw(); //Try To Sign In , On Failed throw exception
	void Register(string username, string password) throw(); //Try To Sign Up , On Failed throw exception
	void Logout(int sock); //Logout by socket
	void Logout(string username); //Logout by username


	string getUsername(int sock); //get username by socket
	int getSock(string username); //get socket by username

	bool isLogin(int sock); //check if the user is login
	bool isLogin(string username); //check if the user is login
	bool isExist(string username); //check if the user exists in the database

private:
	map<int, string> online_users;
	map<string, int> online_users_opposite;
	DataBase& database;

	void add(int sock, string username); //add user/socket to the maps
	void remove(int sock, string username); //remove user/socket from the maps
	bool is_valid_username(int len); //check if the username is valid by its lenght
	bool is_valid_password(int len); //check if the password is valid by its lenght



};

