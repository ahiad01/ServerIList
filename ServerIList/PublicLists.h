#pragma once
#include <string>
#include <mutex>
#include "Exception.h"
#include "Database.h"

using namespace std;

class PublicLists
{
public:
	PublicLists(DataBase* db);
	~PublicLists();

	void addItem(string effector, string group_code, string item_name, string quantity, string remarks); //adding an item to a group
	void removeItem(string effector, string group_code, string item_name); //remove item from group
	void updateItem(string effector, string group_code, string key, string new_name, string new_quantity, string new_remarks); //update the items fields

	vector<vector<string>> getAllItemsInGroup(string group_code);

private:
	DataBase& database;
	mutex mtx;
	bool isUserExistsInGroup(string group_code, string username);
	bool isItemExistsInList(string group_code, string item_name);
	Permission getPermission(string group_code, string username);
	string getQuantity(string group_code, string item_name);
	void incItem(string group_code, string item_name, string inc);
	void decItem(string group_code, string item_name, string dec);

	bool isNumber(string& str);
};
