#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "simulation.h"
#include "inputParser.h"


#define OUTPUT_TO_FILE

using namespace std;

const int startingInterval = 20;
const string inputFile = "input_2h.txt";
const string outputFile = "out.txt";
const double lower = 0.7;
const double upper = 0.9;

void WriteOutput(const vector<shared_ptr<TimeSection>> & sections, ofstream & ofs) {
	
	int interval;
	int time = 0; // -sections.front()->currentInterval;
	int next;
	int last = 0 - sections.front()->currentInterval;
	for (auto it = sections.begin(); it != sections.end(); it++)
	{
		int interval = (*it)->currentInterval;
		next = time + (*it)->GetSectionLength();

		if (last + interval > time){		//case when next train doesn't start right at the begining of the section
			time = last + interval;
		}

		
		while (time < next) {
			
			ofs << "|" << time / 60 << ":" << time % 60 << "|" << endl;
			time += interval;
		}
		last = time - interval;
		time = next;
		
		ofs << "	potential: " << (*it)->potential << endl;
		
	}

}


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


void IncreaseInterval(TimeSectionPtr toChange, const double lowerBound) {
	if (lowerBound > toChange->potential * 2 && toChange->GetSectionLength() >= toChange->currentInterval * 2)
		toChange->currentInterval *= 2;
	else
		toChange->currentInterval++;
	
}

void DecreaseInterval(TimeSectionPtr toChange) {
	//becasue potential is never bigger than 1 i will always take a half of interval (or half+1, when origin is odd)
	if (toChange->currentInterval == 1) {
		
		toChange->state = 3;
		return;
	}
	if (toChange->currentInterval % 2 == 0)
		toChange->currentInterval /= 2;
	else
		toChange->currentInterval = toChange->currentInterval / 2 + 1;
}

shared_ptr< vector<TimeSectionPtr>> SplitTimeSections(vector<TimeSectionPtr>& oldSections) {
	vector<TimeSectionPtr> newSections;
	for (auto it = oldSections.begin(); it != oldSections.end(); it++)
	{
		
		if ((*it)->GetSectionLength() >= (*it)->currentInterval * 2) {
			int half = (*it)->GetSectionLength() / 2;
			if ((*it)->GetSectionLength() % 2 == 0) {
				TimeSection section(half, (*it)->currentInterval);		//new timesections has got by default state = 0
				newSections.push_back(make_shared<TimeSection>(section));
				section = TimeSection(half, (*it)->currentInterval);
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
			TimeSection section((*it)->GetSectionLength(), (*it)->currentInterval);
			section.finalLook= true;		//cannot be splitted and changed interval
			section.state = 0;		//anulating state of interval correction automat
			newSections.push_back(make_shared<TimeSection>(section));
		}
	}
	return make_shared< vector<TimeSectionPtr>>(newSections);
}

void IntervalCorrectionAutomat(TimeSectionPtr toChange, bool increase, const double upperBound, const double lowerBound) {
	if (increase) {
		switch (toChange->state) {
		case -1:
			toChange->state = 2;
			break;
		case -2:
			toChange->state = 3;	//final state
			break;
		case 0 :
			toChange->state = 1;
			break;
		case 1://does not change state...stay in current state
			break;
		case 2://does not change state...stay in current state
			break;
		}
		IncreaseInterval(toChange, lowerBound);		//need only lowerBound	(currentPotential...lower...upper)
		
	}
	else {
		switch (toChange->state) {
		case -1://does not change state...stay in current state
			break;
		case -2://does not change state...stay in current state
			break;
		case 0:
			toChange->state = -1;
			break;
		case 1:
			toChange->state = -2;
			break;
		case 2:
			toChange->state = 3; //final state
			break;
		}
		DecreaseInterval(toChange);		//need only upperBound	(lower...upper...currentPotential)
		
	}
	toChange->potential = 0;
	
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
					if (!(*tIt)->finalLook && (*tIt)->state != 3) {
						//state corresponds with IntervalCorrectionAutomat where 3 means that interval was already decresead and increased or reversly
						if ((*tIt)->potential > upperBound) {
							IntervalCorrectionAutomat((*tIt), false, upperBound, lowerBound);
							readyToSplit = false;
						}
						else if ((*tIt)->potential < lowerBound) {
							if ((*tIt)->currentInterval == (*tIt)->GetSectionLength())	//case when count of passengers is so low that even interval big as time section is not enough to pass to bound
								(*tIt)->state = 3;
							else
								IntervalCorrectionAutomat((*tIt), true, upperBound, lowerBound);
							
							readyToSplit = false;
						}
						else {
							(*tIt)->state = 3;		//when the sections interval is well set 
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
	for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++) {
		(*sIt)->AnulateScheduler();
	}
	SimulateDay(schedulers, dayLength);
}

int main() {
	Parser parser(inputFile);
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
	CreateTimeTable(schedulers, output.first * 60, upper, lower);
	//string s;
	//cin >> s;

#ifdef OUTPUT_TO_FILE
	ofstream myfile;
	myfile.open(outputFile);
	for (auto it = schedulers.begin(); it != schedulers.end(); it++)
	{
		myfile << "Line: " << (*it)->GetLineId() << endl;
		WriteOutput((*it)->timeSections, myfile);
		myfile << endl;
	}
	myfile.close();
#endif // OUTPUT_TO_FILE

	return 0;
}

