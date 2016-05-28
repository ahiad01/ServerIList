#pragma once
#include <vector>
#include <string>
#include "Database.h"

using namespace std;

struct GroupData
{
	string code;
	string name;
};
struct UserData
{
	string name;
	string status;
};
struct ItemData
{
	string name;
	string quantity;
	string remarks;
};


class Restore
{
public:
	Restore(DataBase *db);
	~Restore();

	string buildRestoreMessage(string username);

	

private:
	DataBase& m_database;

	vector<vector<string>> m_groups;
	vector<vector<string>> m_users;
	vector<vector<string>> m_items;
	int m_index_g;
	int m_index_u;
	int m_index_i;

	void findGroupsOfUser(string username);
	void findUsersInGroup(string group_code);
	void findItemsInGroup(string group_code);

	GroupData getNextGroup();
	UserData getNextUser();
	ItemData getNextItem();

	bool eof_groups();
	bool eof_users();
	bool eof_items();

	void removeLastChar(string& st);
};

/*while(!Eof)
{
	group = getNextGroup();
	ret += group.code + "," + group.name + ",";
	findUsersInGroup(group.code)
	while(Eof User)
	{
		user = getNextUser();
		ret += user.name + ":" + user.status;
		ret += "-";	
	}
    removeLastChar(ret);
    ret += ",";
    
    findItemsInGroup(group.code);
    while(Eof Items)
    {
        item = getNextItem();
        ret += item.name + ":" + item.quantity + ":" + item.remarks;
        ret += "-";
    }
    removeLastChar(ret);
    ret += "|";
	

}
ret += "|";*/
/*@116|code,groupname,user1:status-user2:status,item1:quantity:remarks-item2:quantity:remarks|code,grupname,user.....||
*/