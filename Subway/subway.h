#ifndef SUBWAY_H
#define SUBWAY_H

#include <string>
#include <set>
#include <deque>

class Station;
using StationPtr = std::shared_ptr<Station>;
class Train;
using TrainPtr = std::shared_ptr<Train>;
class Line;
using LinePtr = std::shared_ptr<Line>;

class Line {
public:
	Line(int i, int num) : id(i), numberOfStations(num) {}
	int GetID() { return id; }
	int GetnumberOfStations() { return numberOfStations; }
	std::deque<StationPtr> stations;
	std::deque<TrainPtr> onTheWay;
private:
	int id;
	int numberOfStations;
};


class Station {
public:
	Station(int i, int freq, bool transf, std::string s) :id(i), frequency(freq), transferStation(transf), name(s) {}
	int AcceptPassengers(int value);
	int GeneratePassengers();
	int GetFrequency() { return frequency; }
	int GetID() { return id; }
	std::string GetName(){ return name; }
	bool IsTransferStation() { return transferStation; }
	StationPtr next;
	StationPtr prev;
	std::set<StationPtr> transfers;
private:
	int id;
	int frequency;
	bool transferStation;
	std::string name;
};


class Train {
public:
	void MoveNext();
	int ReleasePassengers();
	void AcceptPassengers(int value);
	StationPtr current;
private:
	bool forwardDirection;
	int passengers;
};



#endif 
