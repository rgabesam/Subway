#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>

#include "subway.h"

class Parser {
public:
	Parser(std::string file) : fileName(file) { ifs = std::ifstream(file); }
	std::shared_ptr<Line> ParseSubwayLine();
private:
	void ReadLine(); //put whole line into stringstream, and also clear whole ss
	std::string& ReadWorld();	//return one world of line or \n at the end
	StationPtr ParseStation();
	std::ifstream ifs;
	std::stringstream ss;
	std::string fileName;
};


class InputErrorException : public std::exception {
public:
	InputErrorException(std::string msg) : message(msg) {}
	virtual const char *what() const { return message.c_str();
	}
private:
	std::string message;
};

#endif 