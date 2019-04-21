#include "subway.h"

#include <map>
#include <algorithm>

using namespace std;

int RandomInt(int upperBound) {		//generate random int from 0 to upperBound
	return (rand() % upperBound);
}



void Train::PassangersOnOff()
{
	GetOff();
	GetOn();
}

void Train::GetOn()
{
	int gettingOn;
	if (forwardDirection) {			//just passengers to stations with id > this.station.id
		for (int i = station->id + 1; i <= passengers.size(); i++)
		{
			gettingOn = min(FreeSpace(), station->waiting.at(i));
			passengers.at(i) += gettingOn;
			passengersCount += gettingOn;
			station->waiting.at(i) -= gettingOn;
			if (FreeSpace() == 0)
				return;
		}
	}
	else {
		for (int i = station->id - 1; i >= 1; i--)
		{
			gettingOn = min(FreeSpace(), station->waiting.at(i));
			passengers.at(i) += gettingOn;
			passengersCount += gettingOn;
			station->waiting.at(i) -= gettingOn;
			if (FreeSpace() == 0)
				return;
		}
	}
}

void Train::GetOff()
{
	int stationID = station->id;
	int leaving = passengers.at(stationID);
	passengersCount -= leaving;
	passengers.at(stationID) = 0;
}


int Station::AcceptPassengers(int value)
{
	return 0;
}

void Station::AddPassengers(std::vector<int> & probability)
{
	int dest;
	for (int i = 0; i < passengersPerMinute; i++)
	{
		dest = id;
		while (dest == id) {
			dest = probability[RandomInt(probability.size())];
		}
		waiting.at(id)++;
	}
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

Train::Train(int maxCapacity, TimeSectionPtr timeSection, bool forward, StationPtr startingStation, int distance)
	: capacity(maxCapacity), start(timeSection), passengersCount(0), forwardDirection(forward), station(startingStation), remainsToNext(distance) 
{
	for (int i = 1; i <= startingStation->waiting.size(); i++)
	{
		passengers.emplace(make_pair(i, 0));
	}
}
