#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>

class Simulator;
using SimulatorPtr = std::shared_ptr<Simulator>;

class Scheduler;
using SchedulerPtr = std::shared_ptr<Scheduler>;

class TimeSection;
using TimeSectionPtr = std::shared_ptr<TimeSection>;

class Simulator {
public:
private:
	int dayLength;		//in minutes, but input comes in hours

};


class Scheduler {
public:
private:
	std::vector<int> amountOfPeople;
};


class TimeSection {
public:
	TimeSection() {}
	int GetSectionLength() { return sectionLength; }
	double potential;		//real number between 0 and 1  ... if potential ==1 it means that in concreate time section is any train crowded
private:
	int sectionLength;		//in minutes
};



#endif