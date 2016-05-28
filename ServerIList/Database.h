#pragma once
#include <string>
#include <vector>
#include <time.h>
#include "sqlite3.h"
//#include "md5.h"

#define DB_NAME "ILIST_SERVER_DATABASE.db"

using namespace std;

struct Permission
{
	string permission_string;
	int permission_level;

	Permission(string arg1, int arg2) : permission_string(arg1), permission_level(arg2) {}
	bool operator==(Permission& other) { return other.permission_level == permission_level; }
};

class DataBase
{
public:
	DataBase();
	~DataBase();

	bool open(); //open the database
	vector<vector<string>> query(const string query); //exec a query 
	void close();



	static std::string get_current_time(); //get time now as string
	static vector<vector<string>> Empty; //Empty result
	
	/*Print tables*/
	void printUsersTable();
	void printPublicGroupsTable();
	void printUsersInGroups();
	void printLists();
	void printUsersPackets();

private:
	sqlite3 *database;
	//MD5 md5;


	void create_table(string sql);
	void drop_table(string table);

};

class SqlQuerys
{
public:
	const std::string PublicGroups() { return m_PublicGroups; }
	const std::string Users() { return m_Users; }
	const std::string Users_In_Groups() { return m_Users_In_Groups; }
	const std::string Lists() { return m_Lists; }
	const std::string UsersPackets() { return m_UsersPackets; }
private:
	const std::string m_PublicGroups = "CREATE TABLE IF NOT EXISTS PublicGroups"
	"("
	"code			 INTEGER PRIMARY KEY   AUTOINCREMENT,"
	"name			 TEXT				   NOT NULL,"
	"admin			 TEXT,"
	"creation_date   TEXT			       NOT NULL,"
	"number_of_users INTEGER			   NOT NULL,"
	"FOREIGN KEY(admin) REFERENCES Users(username)"
	");";

	const std::string m_Users = "CREATE TABLE IF NOT EXISTS Users"
		"("
		"username TEXT PRIMARY KEY NOT NULL,"
		"password TEXT			   NOT NULL"
		");";

	const std::string m_Users_In_Groups = "CREATE TABLE IF NOT EXISTS Users_In_Groups"
		"("
		"group_code INTEGER NOT NULL,"
		"username TEXT NOT NULL,"
		"status TEXT NOT NULL,"
		"PRIMARY KEY(group_code, username),"
		"FOREIGN KEY(group_code) REFERENCES PublicGroups(code),"
		"FOREIGN KEY(username) REFERENCES Users(username)"
		");";

	const std::string m_Lists = "CREATE TABLE IF NOT EXISTS Lists"
		"("
		"group_code INTEGER NOT NULL,"
		"item_name TEXT NOT NULL,"
		"quantity INTEGER,"
		"remarks TEXT,"
		"PRIMARY KEY(group_code, item_name),"
		"FOREIGN KEY(group_code) REFERENCES PublicGroups(code)"
		");";

	const std::string m_UsersPackets = "CREATE TABLE IF NOT EXISTS UsersPackets"
		"("
		"code	INTEGER PRIMARY KEY   AUTOINCREMENT,"
		"username TEXT NOT NULL,"
		"message TEXT NOT NULL,"
		"FOREIGN KEY(username) REFERENCES Users(username)"
		");";
};

class Permissions
{
public:
	static Permission Admin;
	static Permission FullControl;
	static Permission ReadWrite;
	static Permission Read;
	static Permission Non;
	static bool isAuthorized(string user_permission, Permission requested_permission);
	static bool isAuthorized(Permission user_permission, Permission requested_permission);
	static Permission getPermission(string user_permission);
};