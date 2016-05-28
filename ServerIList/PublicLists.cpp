#include "PublicLists.h"


PublicLists::PublicLists(DataBase* db) : database(ref(*db))
{}


PublicLists::~PublicLists()
{
}

void PublicLists::addItem(string effector, string group_code, string item_name, string quantity, string remarks)
{
	string sql;
	Permission status("",0);

	mtx.lock();
	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	status = getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::ReadWrite))
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}

	if (this->isItemExistsInList(group_code, item_name))
	{
		incItem(group_code, item_name, quantity);
	}
	else
	{
		sql = "INSERT INTO Lists VALUES(" + group_code + ",'" + item_name + "'," + quantity + ",'" + remarks + "');";
		database.query(sql);
	}
	mtx.unlock();
}

void PublicLists::removeItem(string effector, string group_code, string item_name)
{
	string sql;
	Permission status("", 0);

	mtx.lock();
	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	status = getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::ReadWrite))
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}

	if (!this->isItemExistsInList(group_code, item_name))
	{
		mtx.unlock();
		throw SystemException(); //to do item not found exception
	}

	sql = "DELETE FROM Lists WHERE group_code =" + group_code + " AND item_name ='" + item_name + "';";
	database.query(sql);
	mtx.unlock();
}


void PublicLists::incItem(string group_code, string item_name, string inc)
{
	string quantity = to_string(stoi(getQuantity(group_code, item_name)) + stoi(inc));
	string sql = "UPDATE Lists SET quantity = " + quantity + " WHERE group_code =" + group_code + " AND item_name='" + item_name + "';";
	database.query(sql);
}
void PublicLists::decItem(string group_code, string item_name, string dec)
{
	incItem(group_code, item_name, "-" + dec); //add negetive quantity
}

void  PublicLists::updateItem(string effector, string group_code, string key, string new_name, string new_quantity, string new_remarks)
{
	//key = lastname

	Permission status("", 0);
	string sql;
	string last_name, last_quantity, last_remarks;


	mtx.lock();

	if (this->isUserExistsInGroup(group_code, effector) == false)
	{
		mtx.unlock();
		throw NoSuchUserInTheGroupException();
	}

	status = getPermission(group_code, effector);
	if (!Permissions::isAuthorized(status, Permissions::ReadWrite))
	{
		mtx.unlock();
		throw PermissionIsDeniedException();
	}

	if (!this->isItemExistsInList(group_code, key))
	{
		mtx.unlock();
		throw SystemException(); //to do item not found exception
	}

	sql = "SELECT * FROM Lists WHERE group_code=" + group_code + " AND item_name='" + key + "';";
	auto data = database.query(sql);

	last_name = data[0][1]; //name
	last_quantity = data[0][2];//quantity
	last_remarks = data[0][3]; //quantity

	if (last_name == new_name && last_quantity == new_quantity && last_remarks == new_remarks)
	{
		mtx.unlock();
		throw SystemException(); //the item already exist exception -- no difference 
	}

	if (!isNumber(new_quantity))
	{
		mtx.unlock();
		throw SystemException();
	}

	if (stoi(new_quantity) <= 0)
	{
		mtx.unlock();
		throw SystemException();
	}

	sql = "UPDATE Lists SET "
		"item_name ='" + new_name + "',"
		"quantity =" + new_quantity + ","
		"remarks ='" + new_remarks + "' "
		"WHERE group_code =" + group_code + " AND item_name='" + key + "';";
	
	this->database.query(sql);
	mtx.unlock();
}

bool PublicLists::isUserExistsInGroup(string group_code, string username)
{
	string sql = "SELECT * FROM Users_In_Groups WHERE group_code =" + group_code + " AND username ='" + username + "';";
	return !(database.query(sql) == DataBase::Empty);
}
bool PublicLists::isItemExistsInList(string group_code, string item_name)
{
	string sql = "SELECT * FROM Lists WHERE group_code =" + group_code + " AND item_name ='" + item_name + "';";
	return database.query(sql) != DataBase::Empty;
}
Permission PublicLists::getPermission(string group_code, string username)
{
	string sql = "SELECT * FROM Users_In_Groups WHERE username ='" + username + "' and group_code =" + group_code + ";";
	string status = database.query(sql)[0][2]; //get effector's permission
	return Permissions::getPermission(status);
}
string PublicLists::getQuantity(string group_code, string item_name)
{
	string sql = "SELECT quantity FROM Lists WHERE group_code =" + group_code + " AND item_name ='" + item_name + "';";
	return database.query(sql)[0][0];
}

vector<vector<string>> PublicLists::getAllItemsInGroup(string group_code)
{
	string sql = "SELECT * FROM Lists WHERE group_code=" + group_code + ";";
	return database.query(sql);
}

bool PublicLists::isNumber(string& str)
{
	for each (char c in str)
	{
		if (!isdigit(c))
			return false;
	}
	return true;
}