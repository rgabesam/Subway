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

	//asigning of references between stations
	for (auto it1 = subway.begin(); it1 != subway.end(); it1++)
	{
		int id = 1;
		for (auto it2 = (*it1).second->stations.begin() ; it2 != (*it1).second->stations.end(); it2++)
		{
			if (it2 == (*it1).second->stations.begin()) {		//first station -> prev is null
				(*it2)->next = *(it2 + 1);
				(*it2)->prev = NULL;
			}
			else if (it2 + 1 == (*it1).second->stations.end()) {		//last station -> next is null
				(*it2)->prev = *(it2 - 1);
				(*it2)->next = NULL;
			}
			else {
				(*it2)->next = *(it2 + 1);
				(*it2)->prev = *(it2 - 1);
			}
			(*it2)->id = id;		//setting id
			id++;
		}
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
		auto station = ParseStation(numberOfStations);
		line.stations.push_back(station);
	}

	return make_shared<Line>(line);
}

StationPtr Parser::ParseStation(int numberOfStations)
{
	string name;
	string s;
	int freq;
	

	name = ReadWorld();
	freq = stoi(ReadWorld());

	s = ReadWorld();
	if (s == "()") {
		return make_shared<Station>(Station(name, freq, false, numberOfStations));
	}

	Station station(name, freq, true, numberOfStations);
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