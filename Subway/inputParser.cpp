#include <sstream>
#include <vector>

#include "inputParser.h"

using namespace std;


pair<int, map<int, LinePtr>> Parser::ParseInputFile()
{
	map<int, LinePtr> subway;
	ReadLine();
	int numberOfLines = stoi(ReadWorld());
	int hours = stoi(ReadWorld());
	for (int i = 0; i < numberOfLines; i++)
	{
		auto line = ParseSubwayLine(hours);
		subway.emplace(make_pair(line->GetID(), line));
	}

	return make_pair(hours, subway);
}

shared_ptr<Line> Parser::ParseSubwayLine(int hours)
{
	ReadLine();
	string s;
	
	s = ReadWorld();
	int id = stoi(s);
	
	s = ReadWorld();
	int numberOfStations = stoi(s);

	ReadLine();
	vector<int> passangers;
	for (int i = 0; i < hours; i++)
	{
		passangers.push_back(stoi(ReadWorld()));
	}

	Line line(id, numberOfStations, passangers);

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