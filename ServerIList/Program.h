#pragma once
#include "Manager.h"



class Commands
{
public:
	static string Clients;
	static string LastPackets;
	static string Show;
	static string Hide;
	static string Stop;
	static string Clear;
};

class Program
{
public:
	Program();
	~Program();
	void start();
	void shell();

private:
	Manager* m_manager;
	thread t_shell;

};

