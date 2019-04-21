#ifndef SUBWAY_H
#define SUBWAY_H

#include <string>
#include <set>
#include <deque>

#include "simulation.h"

class Station;
using StationPtr = std::shared_ptr<Station>;
class Train;
using TrainPtr = std::shared_ptr<Train>;
class Line;
using LinePtr = std::shared_ptr<Line>;

const int stationsDistance = 2;

class Line {
public:
	Line(int i, int num, std::vector<int> passangers) : id(i), numberOfStations(num), amountOfPassangers(passangers) {}
	int GetID() { return id; }
	int GetnumberOfStations() { return numberOfStations; }
	std::deque<StationPtr> stations;
	std::deque<TrainPtr> onTheWay;
	std::vector<int> amountOfPassangers;	//number of passengers transported in each hours on this line
private:
	int id;
	int numberOfStations;
};


class Station {
public:
	Station(std::string s, int freq, bool transf) 
		: name(s), frequency(freq), transferStation(transf), nextDistance(stationsDistance), prevDistance(stationsDistance), waiting(0) {}
	int AcceptPassengers(int value);
	void AddPassengers() { waiting += passengersPerMinute; }
	int GetFrequency() { return frequency; }
	int id;
	std::string GetName(){ return name; }
	bool IsTransferStation() { return transferStation; }
	StationPtr next;
	int nextDistance;
	StationPtr prev;
	int prevDistance;
	std::set<StationPtr> transfers;
	std::set<int> transfersToResolve;		//because not every line is parsed when concreate station is parsing so transfers will be resolved later
	int passengersPerMinute;		//how many passengers come every minute
private:
	int waiting;		//number of waiting passangers
	int frequency;
	bool transferStation;
	std::string name;
};

class TimeSection;		//need to be declared because of translation
using TimeSectionPtr = std::shared_ptr<TimeSection>;	
class Train {
public:
	Train(int maxCapacity, TimeSectionPtr timeSection, bool forward, StationPtr startingStation, int distance)
		: capacity(maxCapacity), start(timeSection), passengers(0), forwardDirection(forward), nextStation(startingStation), remainsToNext(distance) {}
	void Move() { remainsToNext--; }
	void PassangersOnOff();
	bool MovingForward() { return forwardDirection; }
	double GetPotential() { return ((double)passengers / capacity); }
	StationPtr nextStation;
	TimeSectionPtr start;		//each train remembers when started to adjust timetable
	int remainsToNext;		//keeps how many minutes remains to the next stations
private:

	bool forwardDirection;
	int passengers;
	int capacity;
	
};



#endif 
