#include "simulation.h"
#include "commonVars.h"

#include <iostream>
#include <memory>

using namespace std;

//#define DEBUG

//const int trainCapacity = 1464;		//https://metroweb.cz/metro/M1/M1.htm	...souprava M1 v Praze
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
	lineId = currLine->GetID();
}

void Scheduler::AnulateScheduler() 
{
	for (auto it = timeSections.begin(); it != timeSections.end(); it++)
	{
		(*it)->potential = 0;
	}
	currentTime = 0;
	timeSectionsIndex = 0;
	lastTrain = timeSections.at(timeSectionsIndex)->currentInterval;
	endOfCurrentTimeSection = timeSections.at(timeSectionsIndex)->GetSectionLength() - 1;
}

void Scheduler::SimulateMinute()
{
#ifdef DEBUG
	cout << "DEBUG:		minute " << currentTime << endl;

#endif // DEBUG

	if (currentTime % 60 == 0)				
		DistributePassengers(line->amountOfPassangers[currentTime / 60]);		//setting of amount of generating passengers 
	GeneratePassengers();		
	MoveTrains();		
	
	if (lastTrain >= timeSections.at(timeSectionsIndex)->currentInterval) {		
		/*
		unequality is there for case when previous time section had bigger interval than current and 
		lastTrain is already bigger than interval of current time section
		*/
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
		(*it)->AddPassengers(line->probabilityMap, (*it)->passengersPerMinute);
	}
}

void Scheduler::MoveTrains()
{
	for (auto it = line->onTheWayFront.begin(); it != line->onTheWayFront.end(); it++)
	{
		(*it)->Move();
	}
	for (auto it = line->onTheWayBack.begin(); it != line->onTheWayBack.end(); it++)
	{
		(*it)->Move();
	}
}

void Scheduler::AddTrains()
{
	Train t1(trainCapacity, timeSections.at(timeSectionsIndex), true, line->stations.front(), line->stations.front()->nextDistance);
	Train t2(trainCapacity, timeSections.at(timeSectionsIndex), false, line->stations.back(), line->stations.back()->prevDistance);

	line->onTheWayFront.push_back(make_shared<Train>(t1));
	line->onTheWayBack.push_back(make_shared<Train>(t2));
#ifdef DEBUG
	cout << "DEBUG:		just started new train at " << line->stations.front()->GetName() << endl;
	cout << "DEBUG:		just started new train at " << line->stations.back()->GetName() << endl;
#endif // DEBUG

	

}

void Scheduler::ServiceTrains()
{
	bool erased = false;		//if i erase train which arrived to the last station i need to decrement iterator
	//because i use deque, i knwo that when i erase a train it is first on the deque so i couldn't decrement iterator instantly in the current loop iteration

	for (auto it = line->onTheWayFront.begin(); it != line->onTheWayFront.end(); it++)
	{
		if (erased) {
			erased = false;
			it--;
		}
		if ((*it)->remainsToNext == 0) {
			(*it)->PassangersOnOff();

			//setting the potential of the time section
			double potential = (*it)->GetPotential();
			//if (potential == 1)
			//	cout << "---------WARNING: POTENTIAL = 1---------" << endl;
			if (potential > (*it)->start->potential)
				(*it)->start->potential = potential;

			//setting next station
			(*it)->remainsToNext = (*it)->station->nextDistance;
			(*it)->station = (*it)->station->next;
			if ((*it)->station == NULL) {			//removing trains at the last station
				it = line->onTheWayFront.erase(it);
				erased = true;
			}
			
		}

		if (it == line->onTheWayFront.end())
			break;
		//just protection for case when it erase the last train which is last in the deque so iterator is end() ..then it would crash if it tried to increment iterator
	}

	erased = false;

	for (auto it = line->onTheWayBack.begin(); it != line->onTheWayBack.end(); it++) {
		if (erased) {
			erased = false;
			it--;
		}
		if ((*it)->remainsToNext == 0) {
			(*it)->PassangersOnOff();

			double potential = (*it)->GetPotential();
			
			if (potential > (*it)->start->potential)
				(*it)->start->potential = potential;

			
			(*it)->remainsToNext = (*it)->station->prevDistance;
			(*it)->station = (*it)->station->prev;
			if ((*it)->station == NULL) {	//removing trains at the last station
				it = line->onTheWayBack.erase(it);
				erased = true;
			}
			
		}

		if (it == line->onTheWayBack.end())
			break;
	}
}

void Scheduler::ScheduleNextTimeSection()
{
	
	timeSectionsIndex++;
	endOfCurrentTimeSection += timeSections.at(timeSectionsIndex)->GetSectionLength();		//what time ends new scheduled time section
}
