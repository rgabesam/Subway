#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include "simulation.h"
#include "inputParser.h"

using namespace std;

const int startingInterval = 20;


void SimulateDay(vector<SchedulerPtr>& schedulers, int dayLength /*in minutes*/) {
	auto schedulersIt = schedulers.begin();
	cout << "Day simulation started with:" << endl;
	for (auto it = schedulers.begin(); it != schedulers.end(); it++)
	{
		cout << "	" << (*it)->timeSections.size() << " time sections" << endl;
	}
	for (int i = 0; i < dayLength; i++)
	{
		//cout << "Simulating minute number " << i + 1 << " of " << dayLength << endl;
		for (schedulersIt = schedulers.begin(); schedulersIt != schedulers.end(); schedulersIt++)
		{
			(*schedulersIt)->SimulateMinute();
		}
	}
}

/*
mode = 1 means that interval is changed just by 1 minute
mode = 2 means that interval is changed * or / 2
increse = true -> interval is increasing
increse = false -> interval is decreasing
*/
void ChangeInterval(TimeSectionPtr toChange, const int mode, const bool increase) {
	if (increase) {
		switch (mode) {
		case 1:
			toChange->currentInterval++;
			break;
		case 2:
			toChange->currentInterval *= 2;
			break;
		}
	}
	else {
		switch (mode) {
		case 1:
			toChange->currentInterval--;
			break;
		case 2:
			if (toChange->currentInterval % 2 == 0)
				toChange->currentInterval /= 2;
			else
				toChange->currentInterval = toChange->currentInterval / 2 + 1;
			break;
		}
	}
	toChange->potential = 0;
}

shared_ptr< vector<TimeSectionPtr>> SplitTimeSections(vector<TimeSectionPtr>& oldSections) {
	vector<TimeSectionPtr> newSections;
	for (auto it = oldSections.begin(); it != oldSections.end(); it++)
	{
		if ((*it)->GetSectionLength() >= (*it)->currentInterval * 2) {
			int half = (*it)->GetSectionLength() / 2;
			if ((*it)->GetSectionLength() % 2 == 0) {
				TimeSection section(half, (*it)->currentInterval);
				newSections.push_back(make_shared<TimeSection>(section));
				newSections.push_back(make_shared<TimeSection>(section));
			}
			else {
				TimeSection section(half, (*it)->currentInterval);
				newSections.push_back(make_shared<TimeSection>(section));
				section = TimeSection(half + 1, (*it)->currentInterval);	//because odd int divided by two is rounded down
				newSections.push_back(make_shared<TimeSection>(section));
			}
		}
		else {
			(*it)->finalLook = true;		//cannot be splitted and changed interval
			newSections.push_back(*it);
		}
	}
	return make_shared< vector<TimeSectionPtr>>(newSections);
}

//upperBound and lowerBound are bounds of used potential of trains, they are between 0 and 1 according to percents
void CreateTimeTable(vector<SchedulerPtr>& schedulers, int dayLength /*in minutes*/, double upperBound, double lowerBound) {
	bool ready = false;
	while (!ready)
	{
		bool readyToSplit = false;
		while (!readyToSplit) {
			readyToSplit = true;		//if any time sections interval is changed readyToSplit is set to false;
			for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++) {
				(*sIt)->AnulateScheduler();
			}
			SimulateDay(schedulers, dayLength);
			for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++)//foreach scheduler ~ foreach line
			{
				for (auto tIt = (*sIt)->timeSections.begin(); tIt != (*sIt)->timeSections.end(); tIt++)	//foreach time section
				{
					if (!(*tIt)->finalLook) {
						if ((*tIt)->potential > upperBound) {
							ChangeInterval((*tIt), 2, false);
							readyToSplit = false;
						}
						else if ((*tIt)->potential < lowerBound) {
							ChangeInterval((*tIt), 1, true);
							readyToSplit = false;
						}
						
					}
				}
			}
		}

		ready = true; //if any time section is splitted ready is set to false;
		for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++)//foreach scheduler ~ foreach line
		{
			auto newSections = *(SplitTimeSections((*sIt)->timeSections));
			if (newSections.size() != (*sIt)->timeSections.size())
				ready = false;
			(*sIt)->timeSections = newSections;
		}
		
		
	}
}

int main() {
	Parser parser("input.txt");
	auto output = parser.ParseInputFile();
	auto subway = output.second;
	cout << "number of links is :" << subway.size() << endl;
	cout << "subway is working " << output.first << " hours per day" << endl;

	
	vector<SchedulerPtr> schedulers;
	//TimeSection section(output.first * 30, 1);
	
	//section = TimeSection(output.first * 30, 2);
	//sections.push_back(make_shared<TimeSection>(section));
	for (auto it = subway.begin(); it != subway.end(); it++)
	{
		TimeSection section(output.first * 60, startingInterval);
		vector<TimeSectionPtr> sections;
		sections.push_back(make_shared<TimeSection>(section));
		Scheduler sch(output.first, (*it).second, sections);
		schedulers.push_back(make_shared<Scheduler>(sch));
	}
	CreateTimeTable(schedulers, output.first * 60, 0.85, 0.25);
	//string s;
	//cin >> s;
	return 0;
}

