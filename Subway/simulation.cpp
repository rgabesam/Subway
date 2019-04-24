#include "simulation.h"

#include <iostream>
#include <memory>

using namespace std;

const int trainCapacity = 220;

Scheduler::Scheduler(int hours, LinePtr currLine, std::vector<TimeSectionPtr> sections) : currentTime(0), line(currLine), timeSections(sections)
{
	timeSectionsIt = timeSections.begin();
	dayLength = hours * 60;
	lastTrain = (*timeSectionsIt)->currentInterval;
	frequencySum = 0;
	for (auto it = line->stations.begin(); it != line->stations.end(); it++)
	{
		frequencySum = frequencySum + (*it)->GetFrequency();
	}
	endOfCurrentTimeSection = (*timeSectionsIt)->GetSectionLength() - 1;		//what time ends the first timeSection
	
}

void Scheduler::SimulateMinute()
{
	if (currentTime % 60 == 0)				
		DistributePassengers(line->amountOfPassangers[currentTime / 60]);		//setting of amount of generating passengers 
	GeneratePassengers();		
	MoveTrains();		
	lastTrain++;
	if (lastTrain == (*timeSectionsIt)->currentInterval) {
		lastTrain = 0;
		AddTrains();
	}
	//else {
	//	//lastTrain++;
	//}
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
	Train t1(trainCapacity, *timeSectionsIt, true, line->stations.front(), line->stations.front()->nextDistance);
	Train t2(trainCapacity, *timeSectionsIt, false, line->stations.back(), line->stations.back()->prevDistance);

	line->onTheWay.push_front(make_shared<Train>(t1));
	line->onTheWay.push_back(make_shared<Train>(t2));
}

void Scheduler::ServiceTrains()
{
	for (auto it = line->onTheWay.begin(); it != line->onTheWay.end(); it++)
	{
		if ((*it)->remainsToNext == 0) {
			(*it)->PassangersOnOff();

			//setting the potential of the time section
			double potential = (*it)->GetPotential();
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
	}
}

void Scheduler::ScheduleNextTimeSection()
{
	timeSectionsIt++;
	endOfCurrentTimeSection += (*timeSectionsIt)->GetSectionLength();		//what time ends new scheduled time section
}
