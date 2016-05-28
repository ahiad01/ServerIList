#include "PublicGroup.h"


PublicGroup::PublicGroup(DataBase* db) : database(ref(*db))
{}


PublicGroup::~PublicGroup()
{
}

string PublicGroup::createGroup(string group_name, string admin) throw()
{ 
	string sql;
	string group_code;
	vector<vector<string>> data;
	string time_now = DataBase::get_current_time();
	mtx.lock();


	/*Insert the new group*/
	sql = "INSERT INTO PublicGroups (name, admin, creation_date, number_of_users) "
		"VALUES ('" + group_name + "', '" + admin + "', '" + time_now + "', 1);";
	database.query(sql.c_str()); //insert Values(name, admin, date, numofusers)

	/*Find the code of the new group*/
	sql = "SELECT code FROM PublicGroups ORDER BY code DESC LIMIT 1;"; //get last row
	if ((data = database.query(sql)) == DataBase::Empty)
	{
		mtx.unlock();
		throw FailedCreateGroupException();
	}
	group_code = (data)[0][0];


	/*Insert an admin to Users in groups*/
	string status = "admin";
	sql = "INSERT INTO Users_In_Groups VALUES(" + group_code + ",'" + admin + "', '" + status + "');"; //Insert Values(group_code, username, status)
	database.query(sql);


	mtx.unlock();
	return group_code; // return the group_code
}

void PublicGroup::removeGroup(string effector, string group_code) throw()
{
	string sql;
	vector<vector<string>> data;
	Permission status("",0);

	mtx.lock();

	/*Checking Effector*/
	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}


	status = this->getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::Admin)) //only admin can remove group
	{
		mtx.unlock();
		PermissionIsDeniedException("PublicGroup.cpp -- removeGroup()");
	}

	sql = "DELETE FROM PublicGroups WHERE code =" + group_code + ";";
	database.query(sql);

	sql = "DELETE FROM Users_In_Groups WHERE group_code =" + group_code + ";";
	database.query(sql);

	sql = "DELETE FROM Lists WHERE group_code =" + group_code + ";";
	database.query(sql);
	
	mtx.unlock();
}

void PublicGroup::addUser(string effector, string group_code, string username, string status) throw()
{
	string sql;
	vector<vector<string>> data;
	int number_of_users;
	Permission effector_status("",0);
	mtx.lock();

	/*Checking Effector*/
	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	/*check number of users*/
	number_of_users = getNumberOfUsersInGroup(group_code);
	if (number_of_users == 4)
	{
		mtx.unlock();
		throw FullGroupException("PublicGroup() -- adduser()");
	}


	if (this->isUserExistsInGroup(group_code, username) == true) //the adding user already this group
	{
		mtx.unlock();
		throw SystemException();
	}

	/*Checking Permission*/
	effector_status = this->getPermission(group_code, effector); //get effector's permission
	if (!Permissions::isAuthorized(effector_status, Permissions::FullControl)) //admin and fc can remove group
	{
		mtx.unlock();
		PermissionIsDeniedException("PublicGroup.cpp -- addUser()");
	}

	if (Permissions::getPermission(status) == Permissions::Non) //Non Permission
	{
		mtx.unlock();
		throw SystemException();
	}

	/*Insert*/
	sql = "INSERT INTO Users_In_Groups VALUES(" + group_code + ",'" + username + "','" + status + "');";
	database.query(sql);
	number_of_users++;

	/*Update*/
	sql = "UPDATE PublicGroups SET number_of_users =" + to_string(number_of_users) + " WHERE code =" + group_code + ";";
	database.query(sql);

	mtx.unlock();
}


void PublicGroup::removeUser(string effector, string group_code, string username) throw()
{
	Permission status("", 0);
	int number_of_users;
	string sql;

	mtx.lock();

	/*Checkig Effector*/
	if (this->isUserExistsInGroup(group_code,effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	/*Checking User*/
	if (this->isUserExistsInGroup(group_code, username) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	/*Checking Permission*/
	status = this->getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::FullControl))
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}
	if (this->getPermission(group_code, username).permission_level == Permissions::Admin.permission_level) //cant remove admin
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}

	number_of_users = this->getNumberOfUsersInGroup(group_code);

	sql = "DELETE FROM Users_In_Groups WHERE group_code=" + group_code + " AND username='" + username + "';";
	database.query(sql);

	sql = "UPDATE PublicGroups SET number_of_users =" + to_string(number_of_users - 1) + " WHERE code =" + group_code + ";";
	database.query(sql);
	mtx.unlock();
}


void PublicGroup::setUserStatus(string effector, string group_code, string username, string new_status) throw()
{ 
	Permission status("", 0);
	int number_of_users;
	string sql;

	mtx.lock();

	/*Checkig Effector*/
	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	/*Checking User*/
	if (this->isUserExistsInGroup(group_code, username) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	/*Checking Permission*/
	status = this->getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::FullControl))
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}
	if (this->getPermission(group_code, username).permission_level == Permissions::Admin.permission_level) //cant set permission of admin
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}

	sql = "UPDATE Users_In_Groups SET status ='" + new_status + "' WHERE group_code =" + group_code + " AND username ='" + username + "';";
	database.query(sql);
	mtx.unlock();
}

vector<vector<string>> PublicGroup::getAllUsersInGroup(string group_code)
{ 
	string sql = "SELECT group_code,username,status FROM Users_In_Groups WHERE group_code =" + group_code + ";";
	return database.query(sql);
}

vector<vector<string>> PublicGroup::getAllGroupsOfUser(string username)
{
	string sql = "SELECT * FROM Users_In_Groups WHERE username ='" + username + "';";

	return database.query(sql);
}


bool PublicGroup::isUserExistsInGroup(string group_code, string username)
{
	string sql = "SELECT * FROM Users_In_Groups WHERE username ='" + username + "' and group_code =" + group_code + ";";

	if (database.query(sql) == DataBase::Empty) //the adding user already this group
	{
		return false;
	}
	else
		return true;
}

int PublicGroup::getNumberOfUsersInGroup(string group_code)
{
	string sql = "SELECT number_of_users FROM PublicGroups WHERE code =" + group_code + ";";
	int number_of_users = stoi(database.query(sql)[0][0]);
	return number_of_users;
}

Permission PublicGroup::getPermission(string group_code, string username)
{
	string sql = "SELECT * FROM Users_In_Groups WHERE username ='" + username + "' and group_code =" + group_code + ";";
	string status = database.query(sql)[0][2]; //get effector's permission
	return Permissions::getPermission(status);
}

string PublicGroup::getGroupName(string group_code)
{
	string sql = "SELECT name from PublicGroups WHERE code=" + group_code;
	return database.query(sql)[0][0];
}