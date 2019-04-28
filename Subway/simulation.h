#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>

#include "subway.h"

class Scheduler;
using SchedulerPtr = std::shared_ptr<Scheduler>;

class TimeSection;
using TimeSectionPtr = std::shared_ptr<TimeSection>;


class TimeSection {
public:
	TimeSection(int length, int startingInterval) : sectionLength(length), currentInterval(startingInterval) , potential(0), finalLook(false), state(0) {}
	int GetSectionLength() { return sectionLength; }
	double potential;		//real number between 0 and 1  ... if potential ==1 it means that in concreate time section is at least one train crowded
	int currentInterval;		//holds gap in minutes between trains 
	bool finalLook;			//while(false) timeSection can be modified in terms of interval and spliting
	int state;			//to protect against cycles when adjusting cycles
private:
	int sectionLength;		//in minutes
};



class Line;		//need to be declared because of translation
using LinePtr = std::shared_ptr<Line>;		
class Station;
using StationPtr = std::shared_ptr<Station>;
class Scheduler {
public:
	Scheduler(int hours, LinePtr currLine, std::vector<TimeSectionPtr> sections);
	void SimulateMinute();
	void AnulateScheduler();		//anulate all values make the scheduler ready to simulate another day
	std::vector<TimeSectionPtr> timeSections;
	//std::vector<TimeSectionPtr>::iterator timeSectionsIt;
	int timeSectionsIndex;
	bool IsEnd() { return (currentTime + 1 == dayLength); }
private:
	void DistributePassengers(int passengers);		//at the begining of each hour distribute number of passengers to each line depending on the frequency of the station
	void GeneratePassengers();		//each minute add fraction of passengers to each station
	void MoveTrains();		//move every train on the way
	void AddTrains();		//add new trains on the way
	void ServiceTrains();		//load and release passangers from the train
	void ScheduleNextTimeSection();		//just switches to the next timeSection if needed
	int lastTrain;		//last train started before lastTrain minutes
	int endOfCurrentTimeSection;		//to controll sections
	int dayLength;		//in minutes, but input comes in hours
	int currentTime;		//count number of minutes gone
	int frequencySum;		
	LinePtr line;
};



#endif