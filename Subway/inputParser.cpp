#include <sstream>

#include "inputParser.h"

using namespace std;

void Parser::ReadLine()
{
	ss.clear();
	string line;
	getline(ifs, line);
	ss << line;
}

std::string Parser::ReadWorld()
{
	string ret;
	if (ss.good())
		ss >> ret;
	else
		return "\n";
	return ret;
}

shared_ptr<Line> Parser::ParseSubwayLine()
{
	ReadLine();
	string s;
	
	s = ReadWorld();
	int id = stoi(s);
	
	s = ReadWorld();
	int numberOfStations = stoi(s);

	Line line(id, numberOfStations);

	ReadLine();
	for (int i = 0; i < numberOfStations; i++)
	{
		auto station = ParseStation();
		line.stations.push_front(station);
	}

	return make_shared<Line>(line);
}

StationPtr Parser::ParseStation()
{
	return StationPtr();
}
