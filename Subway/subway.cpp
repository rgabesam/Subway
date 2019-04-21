#include "subway.h"

#include <map>

using namespace std;



void Train::PassangersOnOff()
{
}


int Station::AcceptPassengers(int value)
{
	return 0;
}

void Station::AddPassengers(std::vector<int> & probability)
{

}

Station::Station(std::string s, int freq, bool transf, int numberOfStations)
	: name(s), frequency(freq), transferStation(transf), nextDistance(stationsDistance), prevDistance(stationsDistance)
{
	for (int i = 1; i <= numberOfStations; i++)
	{
		waiting.emplace(make_pair(i, 0));
	}
}


Line::Line(int i, int num, std::vector<int> passangers) : id(i), numberOfStations(num), amountOfPassangers(passangers) 
{
	for (auto it = stations.begin(); it != stations.end(); it++)
	{
		int id = (*it)->id;
		for (int i = 0; i < (*it)->GetFrequency(); i++)
		{
			probabilityMap.push_back(id);
		}
	}
}
