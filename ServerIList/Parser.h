#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "Config.h"
#include "Exception.h"
using namespace std;

class Parser
{
public:
	Parser(string msg);
	~Parser();

	//getters
	int getMsgCode();
	vector<string> getArguments();

private:
	string message;
	vector<string> args;
	int msgCode;
	void split(string& str, char ch, vector<string>& args);
	bool same_char_in_a_row(string& str, char ch); //check if there is same char twice in a row
};

