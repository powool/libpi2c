
#include "checkTerminate.hpp"

checkException::checkException(const std::string &e) : msg(e) {;}

const char* checkException::what() { return msg.c_str(); }


bool checkTerminate::dieFlag = false;

void checkTerminate::interrupt(int sig) { terminate(); }

void checkTerminate::terminate() { dieFlag = true; }

void checkTerminate::check () {
	if(dieFlag) throw checkException("program shutdown from SIGINT");
}
