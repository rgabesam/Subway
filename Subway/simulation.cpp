#include "simulation.h"

#include <iostream>
#include <memory>

using namespace std;

const int trainCapacity = 1464;		//https://metroweb.cz/metro/M1/M1.htm	...souprava M1 v Praze
//const int trainCapacity = 1526		//https://metroweb.cz/metro/metro/stanice/linka_a.htm		...souprava 81–71M

Scheduler::Scheduler(int hours, LinePtr currLine, std::vector<TimeSectionPtr> sections) : currentTime(0), line(currLine), timeSections(sections)
{
	timeSectionsIndex = 0;
	dayLength = hours * 60;
	lastTrain = timeSections.at(timeSectionsIndex)->currentInterval;
	frequencySum = 0;
	for (auto it = line->stations.begin(); it != line->stations.end(); it++)
	{
		frequencySum = frequencySum + (*it)->GetFrequency();
	}
	endOfCurrentTimeSection = timeSections.at(timeSectionsIndex)->GetSectionLength() - 1;		//what time ends the first timeSection
	
}

void Scheduler::AnulateScheduler() 
{
	currentTime = 0;
	timeSectionsIndex = 0;
	lastTrain = timeSections.at(timeSectionsIndex)->currentInterval;
	endOfCurrentTimeSection = timeSections.at(timeSectionsIndex)->GetSectionLength() - 1;
}

void Scheduler::SimulateMinute()
{
	if (currentTime % 60 == 0)				
		DistributePassengers(line->amountOfPassangers[currentTime / 60]);		//setting of amount of generating passengers 
	GeneratePassengers();		
	MoveTrains();		
	
	if (lastTrain == timeSections.at(timeSectionsIndex)->currentInterval) {
		lastTrain = 0;
		AddTrains();
	}
	lastTrain++;
	
	ServiceTrains();

	currentTime++;

	if (currentTime == endOfCurrentTimeSection && !IsEnd())
		ScheduleNextTimeSection();
}

void Scheduler::DistributePassengers(int passengers)
{
	int passengersFraction = passengers / frequencySum;
	for (auto it = line->stations.begin(); it != line->stations.end(); it++)
	{
		(*it)->passengersPerMinute = ((*it)->GetFrequency() * passengersFraction) / 60;
	}
}

void Scheduler::GeneratePassengers()
{
	for (auto it = line->stations.begin(); it != line->stations.end(); it++)
	{
		(*it)->AddPassengers(line->probabilityMap);
	}
}

void Scheduler::MoveTrains()
{
	for (auto it = line->onTheWay.begin(); it != line->onTheWay.end(); it++)
	{
		(*it)->Move();
	}
}

void Scheduler::AddTrains()
{
	Train t1(trainCapacity, timeSections.at(timeSectionsIndex), true, line->stations.front(), line->stations.front()->nextDistance);
	Train t2(trainCapacity, timeSections.at(timeSectionsIndex), false, line->stations.back(), line->stations.back()->prevDistance);

	line->onTheWay.push_front(make_shared<Train>(t1));
	line->onTheWay.push_back(make_shared<Train>(t2));

	//cout << "		just started new train at " << line->stations.front()->GetName() << endl;
	//cout << "		just started new train at " << line->stations.back()->GetName() << endl;

}

void Scheduler::ServiceTrains()
{
	for (auto it = line->onTheWay.begin(); it != line->onTheWay.end(); it++)
	{
		if ((*it)->remainsToNext == 0) {
			(*it)->PassangersOnOff();

			//setting the potential of the time section
			double potential = (*it)->GetPotential();
			if (potential == 1)
				cout << "---------WARNING: POTENTIAL = 1---------" << endl;
			if (potential > (*it)->start->potential)
				(*it)->start->potential = potential;

			//setting next station
			if ((*it)->MovingForward()) {		//there is difference between trains with different direction		
				(*it)->remainsToNext = (*it)->station->nextDistance;
				(*it)->station = (*it)->station->next;
				if ((*it)->station == NULL)			//removing trains at the last station
					it = line->onTheWay.erase(it);
			}
			else {
				(*it)->remainsToNext = (*it)->station->prevDistance;
				(*it)->station = (*it)->station->prev;
				if ((*it)->station == NULL)			//removing trains at the last station
					it = line->onTheWay.erase(it);
			}
		}

		if (it == line->onTheWay.end())		
			break;
		//just protection for case when it erase the last train which is last in the deque so iterator is end() ..then it would crash if it tried to increment iterator
	}
}

void Scheduler::ScheduleNextTimeSection()
{
	timeSectionsIndex++;
	endOfCurrentTimeSection += timeSections.at(timeSectionsIndex)->GetSectionLength();		//what time ends new scheduled time section
}
