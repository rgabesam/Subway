#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

#include "subway.h"

class Parser {
public:
	Parser(std::string file) : fileName(file) { ifs = std::ifstream(file); }
	std::pair<int, std::shared_ptr<std::map<int, LinePtr>>>  ParseInputFile();
private:
	std::shared_ptr<Line> ParseSubwayLine(int hours);
	void ReadLine(); //put whole line into stringstream, and also clear whole ss
	std::string ReadWord();	//return one world of line or \n at the end
	StationPtr ParseStation(int numberOfStations);
	void ResolveTransfers(std::map<int, LinePtr> & subway);
	std::ifstream ifs;
	std::stringstream ss;
	std::string fileName;
};



#endif 
