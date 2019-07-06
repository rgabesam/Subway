#include <sstream>
#include <vector>

#include "inputParser.h"

using namespace std;


pair<int, shared_ptr<map<int, LinePtr>>> Parser::ParseInputFile()
{
	map<int, LinePtr> subway;
	ReadLine();
	int numberOfLines = stoi(ReadWord());
	int hours = stoi(ReadWord());
	for (int i = 0; i < numberOfLines; i++)
	{
		auto line = ParseSubwayLine(hours);
		subway.emplace(make_pair(line->GetID(), line));
	}

	//asigning of references between stations and ids
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

	for (auto it = subway.begin(); it != subway.end(); it++)
	{
		for (auto it2 = (*it).second->stations.begin(); it2 != (*it).second->stations.end(); it2++)
		{
			int id = (*it2)->id;
			for (int i = 0; i < (*it2)->GetFrequency(); i++)
			{
				(*it).second->probabilityMap.push_back(id);
			}
		}
	}

	ResolveTransfers(subway);

	return (make_pair(hours, make_shared< map<int, LinePtr>>(subway)));
}

void  Parser::ResolveTransfers(map<int, LinePtr> & subway) {
	//resolving transfers...it has big complexity, but it is done only once and probably on not big input
	for (auto it = subway.begin(); it != subway.end(); it++)
	{
		for (auto it2 = (*it).second->stations.begin(); it2 != (*it).second->stations.end(); it2++)
		{
			if ((*it2)->IsTransferStation()) {
				for (auto it3 = (*it2)->transfersToResolve.begin(); it3 != (*it2)->transfersToResolve.end(); it3++)
				{
					auto line = subway.at(*it3);
					StationPtr station;
					for (auto it4 = line->stations.begin(); it4 != line->stations.end(); it4++)
					{
						if ((*it4)->GetName() == (*it2)->GetName()) {
							station = (*it4);
							break;
						}
					}
					(*it2)->transfers.emplace(make_pair(line, station));
				}
			}
		}
	}
}

shared_ptr<Line> Parser::ParseSubwayLine(int hours)
{
	ReadLine();
	string s;
	
	s = ReadWord();
	int id = stoi(s);
	
	s = ReadWord();
	int numberOfStations = stoi(s);

	ReadLine();
	vector<int> passangers;
	for (int i = 0; i < hours; i++)
	{
		passangers.push_back(stoi(ReadWord()));
	}

	Line line(id, numberOfStations, passangers);

	ReadLine();
	for (int i = 0; i < numberOfStations; i++)
	{
		auto station = ParseStation(numberOfStations);
		line.stations.push_back(station);
	}

	ReadLine();
	vector<int> distances;	//cteni vzadelnosti mezi stanicema
	for (int i = 0; i < numberOfStations - 1; i++)
	{
		distances.push_back(stoi(ReadWord()));
	}

	int q = 0;
	for (auto it = line.stations.begin(); it+1 != line.stations.end(); it++)	//nastavuju vzdalenost v jednom smeru
	{
		(*it)->nextDistance = distances[q];
		q++;
	}
	
	for (auto it = line.stations.rbegin(); it+1 != line.stations.rend(); it++)	//...v druhym smeru
	{
		q--;
		(*it)->prevDistance = distances[q];
	}

	return make_shared<Line>(line);
}

StationPtr Parser::ParseStation(int numberOfStations)
{
	string name;
	string s;
	int freq;
	

	name = ReadWord();
	freq = stoi(ReadWord());

	s = ReadWord();
	if (s == "()") {
		return make_shared<Station>(Station(name, freq, false, numberOfStations));
	}

	Station station(name, freq, true, numberOfStations);
	s = ReadWord();
	while (s != ")") {
		station.transfersToResolve.emplace(stoi(s));
		s = ReadWord();
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

std::string Parser::ReadWord()
{
	string ret;
	if (ss.good())
		ss >> ret;
	else
		ret = "\n";
	return ret;
}