#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdexcept>
#include <string>
#include <fstream>

#include "subway.h"

class Parser {
public:
	Parser(std::string file);
	std::string ReadLine();
	std::shared_ptr<Line> ParseLine();
	StationPtr ParseStation();
private:
	std::ifstream ifs;
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
