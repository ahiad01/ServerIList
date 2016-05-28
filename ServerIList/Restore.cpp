#include "Restore.h"


Restore::Restore(DataBase* db) : m_database(ref(*db))
{}


Restore::~Restore()
{
}

string Restore::buildRestoreMessage(string username)
{
	string ret = "@400|";
	GroupData group;
	UserData user;
	ItemData item;

	findGroupsOfUser(username);

	while (!eof_groups())
	{
		group = getNextGroup();
		ret += group.code + "," + group.name + ",";
		findUsersInGroup(group.code);
		while (!eof_users())
		{
			user = getNextUser();
			ret += user.name + ":" + user.status;
			ret += "-";
		}
		removeLastChar(ret);
		ret += ",";

		findItemsInGroup(group.code);
		while (!eof_items())
		{
			item = getNextItem();
			ret += item.name + ":" + item.quantity + ":" + item.remarks;
			ret += "-";
		}
		removeLastChar(ret);
		ret += "|";
	}
	ret += "|";

	return ret;
}


void Restore::findGroupsOfUser(string username)
{
	string sql = "SELECT group_code FROM Users_In_Groups WHERE username='" + username + "';";

	m_groups = m_database.query(sql);
	m_index_g = 0;
}

void Restore::findUsersInGroup(string group_code)
{
	string sql = "SELECT username,status FROM Users_In_Groups WHERE group_code=" + group_code + ";";

	m_users = m_database.query(sql);
	m_index_u = 0;
	
}

void Restore::findItemsInGroup(string group_code)
{
	string sql = "SELECT item_name,quantity,remarks FROM Lists WHERE group_code=" + group_code + ";";

	m_items = m_database.query(sql);
	m_index_i = 0;
}

GroupData Restore::getNextGroup()
{
	string group_code = m_groups[m_index_g][0];
	GroupData ret;
	string sql = "SELECT code,name FROM PublicGroups WHERE code=" + group_code + ";";
	vector<vector<string>> data = m_database.query(sql);
	ret.code = group_code;
	ret.name = data[0][1];
	m_index_g++;

	return ret;
}

UserData Restore::getNextUser()
{
	UserData ret;
	ret.name = m_users[m_index_u][0];
	ret.status = m_users[m_index_u][1];
	m_index_u++;

	return ret;
}

ItemData Restore::getNextItem()
{
	ItemData ret;
	ret.name = m_items[m_index_i][0];
	ret.quantity = m_items[m_index_i][1];
	ret.remarks = m_items[m_index_i][2];
	m_index_i++;

	return ret;
}

bool Restore::eof_groups()
{
	return m_groups.size() == m_index_g;
}

bool Restore::eof_users()
{
	return m_users.size() == m_index_u;
}

bool Restore::eof_items()
{
	return m_items.size() == m_index_i;
}

void Restore::removeLastChar(string& st)
{
	st = st.substr(0, st.length() - 1);
}