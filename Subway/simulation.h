#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>


//class Simulator;
//using SimulatorPtr = std::shared_ptr<Simulator>;

class Scheduler;
using SchedulerPtr = std::shared_ptr<Scheduler>;

class TimeSection;
using TimeSectionPtr = std::shared_ptr<TimeSection>;

//class Simulator {
//public:
//private:
//	int dayLength;		//in minutes, but input comes in hours
//
//};


class Scheduler {
public:
	Scheduler(int hours, LinePtr currLine, std::vector<int> passengers) : currentTime(0), lastTrain(0), line(currLine), amountOfPassangers(passengers) { dayLength = hours * 60; }
	void SimulateMinute();
private:
	void DistributePassengers(int passengers);		//at the begining of each hour distribute number of passengers to each line depending on the frequency of the station
	void GeneratePassengers();		//each minute add fraction of passengers to each station
	void MoveTrains();		//move every train on the way
	void AddTrains();		//add new trains on the way
	void ServiceTrains();		//load and release passangers from the train
	void ScheduleTimeSection();		//just switches to the next timeSection if needed
	int lastTrain;		//last train started before lastTrain minutes
	int dayLength;		//in minutes, but input comes in hours
	int currentTime;		//count number of minutes gone
	std::vector<int> amountOfPassangers;
	LinePtr line;
};


class TimeSection {
public:
	TimeSection(int length) : sectionLength(length) {}
	int GetSectionLength() { return sectionLength; }
	double potential;		//real number between 0 and 1  ... if potential ==1 it means that in concreate time section is any train crowded
	int currentInterval;		//holds gaps in minutes between trains 
private:
	int sectionLength;		//in minutes
};



#endif