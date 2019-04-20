#include "simulation.h"

void Scheduler::SimulateMinute()
{
	if (currentTime % 60 == 0)				
		DistributePassengers(line->amountOfPassangers[currentTime / 60]);		//setting of amount of generating passengers 
	GeneratePassengers();		
	MoveTrains();		
	if (lastTrain == (*timeSectionsIt)->currentInterval) {
		lastTrain = 0;
		AddTrains();
	}
	else {
		lastTrain++;
	}
	ServiceTrains();

	currentTime++;

	if (currentTime == endOfCurrentTimeSection)
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
		(*it)->AddPassengers();
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
}

void Scheduler::ServiceTrains()
{
}

void Scheduler::ScheduleNextTimeSection()
{
}
