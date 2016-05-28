#pragma once
#include <exception>
#include <string>
using namespace std;

class Exception : public exception
{
public:
	Exception(int code):error_code(code){}
	Exception(int code, string p) : error_code(code), path(p) {}
	virtual const int getErrorCode(){ return error_code; };
	virtual const string where() { return path; }
protected:
	int error_code;
	std::string path;
};

class SystemException : public Exception
{
public:
	SystemException() : Exception(0){}
	virtual const char* what() const throw()
	{
		return "SystemException";
	}
};

class SyntaxErrorException : public Exception
{
public:
	SyntaxErrorException(): Exception(1){}
	virtual const char* what() const throw()
	{
		return "SyntaxErrorException";
	}
};

class SqlInjectionException : public Exception
{
public:
	SqlInjectionException(string path) : Exception(2, path) {}
	virtual const char* what() const throw()
	{
		return "SqlInjectionException";
	}
};

class InvalidCodeNumberException : public Exception
{
public:
	InvalidCodeNumberException() : Exception(3){}
	virtual const char* what() const throw()
	{
		return "InvalidCodeNumberException";
	}
};

class WrongArgumentsNumberException : public Exception
{
public:

	WrongArgumentsNumberException() : Exception(4){}
	virtual const char* what() const throw()
	{
		return "WrongArgumentsNumberException";
	}
};

class InvalidUsernameOrPasswordException : public Exception
{
public:
	InvalidUsernameOrPasswordException() : Exception(5){}
	virtual const char* what() const throw()
	{
		return "InvalidUsernameOrPasswordException";
	}
}; //

class WrongUsernameOrPasswordException : public Exception
{
public:
	WrongUsernameOrPasswordException() : Exception(6){}
	virtual const char* what() const throw()
	{
		return "WrongUernameOrPasswordException";
	}
};

class UserNotAvilableException : public Exception
{
public:
	UserNotAvilableException() : Exception(7){}
	virtual const char* what() const throw()
	{
		return "UserNotAvilableException";
	}
};

class WrongUsernameException : public Exception
{
public:
	WrongUsernameException() : Exception(8){}
	virtual const char* what() const throw()
	{
		return "WrongUsernameException";
	}
};

class FailedCreateGroupException : public Exception
{
public:
	FailedCreateGroupException() : Exception(9){}
	virtual const char* what() const throw()
	{
		return "FailedCreateGroupException";
	}
};

class UserIsOfflineException : public Exception
{
public:
	UserIsOfflineException() : Exception(10){}
	virtual const char* what() const throw()
	{
		return "UserIsOfflineException";
	}
};

class NoSuchUserInTheGroupException : public Exception
{
public:
	NoSuchUserInTheGroupException() : Exception(11){}
	NoSuchUserInTheGroupException(string path) : Exception(11, path){}
	virtual const char* what() const throw()
	{
		return "NoSuchUserInTheGroupException";
	}
};

class PermissionIsDeniedException : public Exception
{
public:
	PermissionIsDeniedException() : Exception(12){}
	PermissionIsDeniedException(string path) : Exception(12, path) {}
	virtual const char* what() const throw()
	{
		return "PermissionIsDeniedException";
	}
};

class FullGroupException : public Exception
{
public:
	FullGroupException(string path) : Exception(13, path) {}
	virtual const char* what() const throw()
	{
		return "FullGroupException";
	}
};