#include "subway.h"

#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

//#define DEBUG_TRAIN
//#define DEBUG_STATION			//..nedoporucuju, vypisuje to kazdou minutu simulace pro kazdou stanici pocty vygenerovanejch cestujicich

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
#ifdef DEBUG_TRAIN
	cout << "DEBUG:		" << leaving << " passengers just arrived to " << station->GetName() << endl;
#endif // DEBUG

	if (station->IsTransferStation()) {
		int transfering = leaving / (station->transfers.size() + 1);
		for (auto it = station->transfers.begin(); it != station->transfers.end(); it++)
		{
			(*it).second->AddPassengers((*it).first->probabilityMap, transfering);
		}

#ifdef DEBUG_TRAIN
		cout << "DEBUG:		" << leaving << "		station is trasfer so " << transfering <<   " people transfer" << endl;
#endif // DEBUG
	}

	passengersCount -= leaving;
	passengers.at(stationID) = 0;
}

void Station::AddPassengers(std::vector<int> & probability, int passengersAmount)
{
#ifdef DEBUG_STATION
	int count = 0;
	for (auto it = waiting.begin(); it != waiting.end(); it++)
	{
		count += it->second;		//total number of waiting passengers
	}
#endif // DEBUG

	int dest;
	for (int i = 0; i < passengersAmount; i++)
	{
		dest = id;
		while (dest == id) {
			dest = probability[RandomInt(probability.size())];
		}
		waiting.at(dest)++;
	}
#ifdef DEBUG_STATION
	cout << "DEBUG: station: " << name <<":original number of passengers " << count << " was increased to " << count + passengersPerMinute << endl;
#endif // DEBUG

}

Station::Station(std::string s, int freq, bool transf, int numberOfStations)
	: name(s), frequency(freq), transferStation(transf), nextDistance(stationsDistance), prevDistance(stationsDistance)		
	//before was distance between stations const but, now it is still initialization with const but afterwards is set explicitly by input
{
	for (int i = 1; i <= numberOfStations; i++)
	{
		waiting.emplace(make_pair(i, 0));
	}
}


Line::Line(int i, int num, std::vector<int> passangers) : id(i), numberOfStations(num), amountOfPassangers(passangers) 
{
	
}

Train::Train(int maxCapacity, TimeSectionPtr timeSection, bool forward, StationPtr startingStation, int distance)
	: capacity(maxCapacity), start(timeSection), passengersCount(0), forwardDirection(forward), station(startingStation), remainsToNext(distance) 
{
	for (int i = 1; i <= startingStation->waiting.size(); i++)
	{
		passengers.emplace(make_pair(i, 0));
	}
}
