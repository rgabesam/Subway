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
	Station(std::string s, int freq, bool transf) : name(s), frequency(freq), transferStation(transf) {}
	int AcceptPassengers(int value);
	void AddPassengers() { waiting += passengersPerMinute; }
	int GetFrequency() { return frequency; }
	//int GetID() { return id; }
	std::string GetName(){ return name; }
	bool IsTransferStation() { return transferStation; }
	StationPtr next;
	StationPtr prev;
	std::set<StationPtr> transfers;
	std::set<int> transfersToResolve;		//because not every line is parsed when concreate station is parsing so transfers will be resolved later
	int passengersPerMinute;		//how many passengers come every minute
private:
	//int id;
	int waiting;		//number of waiting passangers
	int frequency;
	bool transferStation;
	std::string name;
};

class TimeSection;		//need to be declared because of translation
using TimeSectionPtr = std::shared_ptr<TimeSection>;	
class Train {
public:
	Train(int maxCapacity, TimeSectionPtr timeSection, bool forward, StationPtr startingStation)
		: capacity(maxCapacity), start(timeSection), remainsToNext(2), passengers(0), forwardDirection(forward), next(startingStation) {}
	void Move() { remainsToNext--; }
	int ReleasePassengers();
	void AcceptPassengers(int value);
	//std::deque<StationPtr>::iterator next;
	StationPtr next;
	TimeSectionPtr start;		//each train remembers when started to adjust timetable
	int remainsToNext;		//keeps how many minutes remains to the next stations
private:
	bool forwardDirection;
	int passengers;
	int capacity;
	
};



#endif 
