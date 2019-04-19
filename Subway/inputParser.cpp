#include <sstream>

#include "inputParser.h"

using namespace std;


map<int, LinePtr> Parser::ParseInputFile()
{
	map<int, LinePtr> subway;
	ReadLine();
	
	int numberOfLines = stoi(ReadWorld());
	for (int i = 0; i < numberOfLines; i++)
	{
		auto line = ParseSubwayLine();
		subway.emplace(make_pair(line->GetID(), line));
	}

	return subway;
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
	string name;
	string s;
	int freq;
	

	name = ReadWorld();
	freq = stoi(ReadWorld());

	s = ReadWorld();
	if (s == "()") {
		return make_shared<Station>(Station(name, freq, false));
	}

	Station station(name, freq, true);	
	s = ReadWorld();
	while (s != ")") {
		station.transfersToResolve.emplace(stoi(s));
		s = ReadWorld();
	}

	return make_shared<Station>(station);
}


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
		ret = "\n";
	return ret;
}