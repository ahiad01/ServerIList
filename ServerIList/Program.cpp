#include "Program.h"


Program::Program()
{

}


Program::~Program()
{
	delete this->m_manager;
}

void Program::start()
{
	m_manager = new Manager();

}

void Program::shell()
{
	string cmd;
	while (true)
	{
		cin >> cmd;

		if (cmd == "a")
			m_manager->getFreeThreadIndex();

		if (cmd == Commands::Clients)
		{
			cout << "clients size: " << this->m_manager->getServer()->getClients() << endl;
		}
		else if (cmd == Commands::LastPackets)
		{

		}
		else if (cmd == Commands::Show)
		{
			this->m_manager->getServer()->setPrintFlag(true);
		}
		else if (cmd == Commands::Hide)
		{
			this->m_manager->getServer()->setPrintFlag(false);
		}
		else if (cmd == Commands::Stop)
		{
			delete this->m_manager;
			break;
		}
		else if (cmd == Commands::Clear)
		{
			system("cls");
		}
	}
}



string Commands::Clients = string("clients");
string Commands::LastPackets = string("packets");
string Commands::Stop = string("stop");
string Commands::Show = string("show");
string Commands::Hide = string("hide");
string Commands::Clear = string("clear");