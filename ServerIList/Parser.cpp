#include "Parser.h"


Parser::Parser(string msg)
{
	this->message = msg;

	int a;
	if (msg.length() < 4)
		throw SyntaxErrorException();

	if (msg.find("'") != string::npos || msg.find("--") != string::npos)
	{
		throw SqlInjectionException("Parser C'tor");
	}

	if (msg.at(0) != '@' || msg.substr(msg.length() - 2, msg.length()).compare("||") != 0) //structure: @msgcode|arg1|arg2||
		throw SyntaxErrorException();

	if (same_char_in_a_row(msg.substr(0, msg.length() - 1), '|'))
		throw SyntaxErrorException();

	split(msg, '|', args);

	string str_code = args[0].substr(1, args[0].length());

	try{
		this->msgCode = stoi(str_code, nullptr);
		if (FIND_MSG_CODE(this->msgCode) == false)
			throw InvalidCodeNumberException();
	}
	catch (...) //stoi failed 
	{
		throw InvalidCodeNumberException();
	}
	args.erase(args.begin()); //erase first element, i.e - the msg code.
	args.erase(args.end() - 1);
}


Parser::~Parser()
{
}


void Parser::split(string& str, char ch, vector<string>& args)
{
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, ch)) {
		args.push_back(item);
	}
}

int Parser::getMsgCode(){ return msgCode; }
vector<string> Parser::getArguments(){ return args; }

bool Parser::same_char_in_a_row(string& str, char ch)
{
	for (int i = 0; i < str.length() - 1; i++)
	{
		if (str[i] == str[i + 1] && str[i] == ch)
			return true;
	}
	return false;
}