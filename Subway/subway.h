#ifndef SUBWAY_H
#define SUBWAY_H

#include <string>
#include <set>

using StationPtr = std::shared_ptr<Station>;


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
