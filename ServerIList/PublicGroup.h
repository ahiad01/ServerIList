#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include "database.h"
#include "Exception.h"
#include "PublicLists.h"
using namespace std;


class PublicGroup
{
public:
	PublicGroup(DataBase* db);
	~PublicGroup();

	string createGroup(string group_name, string admin) throw(); //create a group with the admin, return the group_code
	void removeGroup(string effector, string group_code) throw(); //delete the group and remove all the user into.
	void addUser(string effector, string group_code, string username, string status) throw(); //add user to the group
	void removeUser(string effector, string group_code, string username) throw(); //remove user from a group
	void setUserStatus(string effector, string group_code, string username, string new_status) throw();//set the premission of user
	vector<vector<string>> getAllUsersInGroup(string group_code); //getting all users

	vector<vector<string>> getAllGroupsOfUser(string username); //getting all groups of user

	string getGroupName(string group_code);

private:


	DataBase& database;
	mutex mtx;

	bool isUserExistsInGroup(string group_code, string username);
	int getNumberOfUsersInGroup(string group_code);
	Permission getPermission(string group_code, string username);


};
