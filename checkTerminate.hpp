#ifndef _CHECK_EXCEPTION_
#define  _CHECK_EXCEPTION_

#include <exception>
#include <string>

class checkException : std::exception {
private:
        std::string     msg;
public:
        checkException(const std::string &e);
        const char* what();
};

class checkTerminate {
	static bool dieFlag;
public:
	static void terminate();
	static void check ();
	static void interrupt(int sig);
};

#endif
