#include "DataBase.h"
#include <iostream>
#include <stdint.h>


vector<vector<string>> DataBase::Empty = vector<vector<string>>(); //Clear vector


DataBase::DataBase()
{ 
	SqlQuerys sq;
	database = NULL; 
	open();
	
	create_table(sq.Users());
	create_table(sq.PublicGroups());
	create_table(sq.Users_In_Groups());
	create_table(sq.Lists());
	create_table(sq.UsersPackets());
	

	//drop_table("Users");
	/*drop_table("PublicGroups");
	drop_table("Users_In_Groups");
	drop_table("Lists");
	drop_table("UsersPackets");*/
}

DataBase::~DataBase()
{
	close();
}

bool DataBase::open()
{
	if (sqlite3_open(DB_NAME, &database) == SQLITE_OK) //can be used to either open existing database files or to create and open new database files
		return true;
	return false;
}

vector<vector<string>> DataBase::query(const string sql)
{
	sqlite3_stmt *statement;
	vector<vector<string>> results;

	const char* query = sql.c_str();

	if (sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK) //the prepared statement that is returned (the sqlite3_stmt object) contains a copy of the original SQL text
	{
		int cols = sqlite3_column_count(statement); //Return the number of columns in the result set returned by the prepared statement.This routine returns 0 if pStmt is an SQL statement that does not return data
		int result = 0;
		while (true)
		{
			result = sqlite3_step(statement);
			//This routine is used to evaluate a prepared statement that has been previously created by the sqlite3_prepare() interface.
			//The statement is evaluated up to the point where the first row of results are available. To advance to the second row of results,
			//invoke sqlite3_step() again. Continue invoking sqlite3_step() until the statement is complete. Statements that do not return
			//results (ex: INSERT, UPDATE, or DELETE statements) run to completion on a single call to sqlite3_step().

			if (result == SQLITE_ROW)
			{
				vector<string> values;
				for (int col = 0; col < cols; col++)
				{
					std::string  val;
					char * ptr = (char*)sqlite3_column_text(statement, col);

					if (ptr)
					{
						val = ptr;
					}
					else val = ""; // this can be commented out since std::string  val;
					// initialize variable 'val' to empty string anyway
					values.push_back(val);  // now we will never push NULL
				}
				results.push_back(values);
			}
			else
				break;
		}
		sqlite3_finalize(statement);
	}

	string error = sqlite3_errmsg(database);
	if (error != "not an error") cout << query << " " << error << endl;


	return results;
}

void DataBase::close()
{
	sqlite3_close(database);
}


void DataBase::create_table(string sql)
{
	query(sql.c_str());
}

void DataBase::drop_table(string table)
{
	this->query("DROP TABLE " + table + ";");
}

void DataBase::printUsersTable()
{
	std::string sql = "SELECT * FROM Users;";
	vector<vector<string>> data = query(sql);
	
	printf("%3s\t%10s\t%10s\n", "[No.]", "Username", "Password");
	printf("____________________________________\n");
	for (int col = 0; col < data.size(); col++) //every line
	{
		printf("%3s\t%10s\t%10s\n", to_string(col).c_str(), data[col][0].c_str(), data[col][1].c_str()); //two fields
	}
}

void DataBase::printPublicGroupsTable()
{
	std::string sql = "SELECT * FROM PublicGroups;";
	vector<vector<string>> data = query(sql);

	printf("%4s\t%10s\t%10s\t%13s\t%15s\n", "code", "name", "admin", "creation date", "number of users");
	printf("_______________________________________________________________________\n");
	for (int col = 0; col < data.size(); col++) //every line
	{
		printf("%4s\t%10s\t%10s\t%13s\t%15s\n", data[col][0].c_str(), data[col][1].c_str(), data[col][2].c_str(), data[col][3].c_str(), data[col][4].c_str()); //five fields
	}
}

void DataBase::printUsersInGroups()
{
	std::string sql = "SELECT * FROM Users_In_Groups;";
	vector<vector<string>> data = query(sql);
	printf("%10s\t%10s\t%10s\n", "group_code", "username", "status");
	printf("_______________________________________\n");
	for (int col = 0; col < data.size(); col++) //every line
	{
		printf("%10s\t%10s\t%10s\n", data[col][0].c_str(), data[col][1].c_str(), data[col][2].c_str()); //three fields
	}
}

void DataBase::printLists()
{
	std::string sql = "SELECT * FROM Lists;";
	vector<vector<string>> data = query(sql);

	printf("%10s\t%10s\t%10s\t%10s\n", "group_code", "item", "quantity", "remarks");
	printf("___________________________________________________________\n");
	for (int col = 0; col < data.size(); col++) //every line
	{
		printf("%10s\t%10s\t%10s\t%10s\n",data[col][0].c_str(), data[col][1].c_str(), data[col][2].c_str(), data[col][3].c_str()); //two fields
	}
}

void DataBase::printUsersPackets()
{
	std::string sql = "SELECT * FROM UsersPackets;";
	vector<vector<string>> data = query(sql);

	printf("%4s\t%10s\t%10s\n", "Code", "Username", "Message");
	printf("____________________________________\n");
	for (int col = 0; col < data.size(); col++) //every line
	{
		printf("%3s\t%10s\t%10s\n", data[col][0].c_str(), data[col][1].c_str(), data[col][2].c_str()); //two fields
	}
}

std::string DataBase::get_current_time()
{
	struct tm timeinfo;
	time_t t = time(0);


	const int MAXLEN = 80;
	char s[MAXLEN];
	localtime_s(&timeinfo, &t);
	strftime(s, MAXLEN, "%d/%m/%Y", &timeinfo);
	return string(s);
}


/*--------------------------------------*/
/*		   Permissions Class		    */
/*--------------------------------------*/

/*Set Static Permissions Values*/
Permission Permissions::Admin = Permission("admin", 4);
Permission Permissions::FullControl = Permission("fc", 3);
Permission Permissions::ReadWrite = Permission("rw", 2);
Permission Permissions::Read = Permission("r", 1);
Permission Permissions::Non = Permission("", 0);


bool Permissions::isAuthorized(string user_permission, Permission requested_permission)
{
	if (isAuthorized(getPermission(user_permission), requested_permission))
		return true;
	else
		return false; //not found permisison
}

bool Permissions::isAuthorized(Permission user_permission, Permission requested_permission)
{
	return user_permission.permission_level >= requested_permission.permission_level;
}

Permission Permissions::getPermission(string user_permission)
{
	if (user_permission == Admin.permission_string)
		return Admin;

	else if (user_permission == FullControl.permission_string)
		return FullControl;

	else if (user_permission == ReadWrite.permission_string)
		return ReadWrite;

	else if (user_permission == Read.permission_string)
		return Read;
	else
		return Non;
}