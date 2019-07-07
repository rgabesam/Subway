#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "simulation.h"
#include "inputParser.h"
#include "commonVars.h"


#define OUTPUT_TO_FILE
//#define DEBUG

using namespace std;

const int startingInterval = 20;

//const string inputFile = "input_2h.txt";
////const string inputFile = "input_8h.txt";
//const string outputFile = "out.txt";
//const double lower = 0.7;
//const double upper = 0.9;
//const int accuracy = 10; //determines how many times can timeSection turn increasing to decreasing or vice versa

int trainCapacity;

string inputFile;
string outputFile;
double lower;
double upper;
int accuracy;

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
		
		//ofs << "	potential: " << (*it)->potential << endl;
		
	}

}


void SimulateDay(vector<SchedulerPtr>& schedulers, int dayLength /*in minutes*/) 
{
	auto schedulersIt = schedulers.begin();

#ifdef DEBUG
	cout << "DEBUG:		Day simulation started with:" << endl;
	for (auto it = schedulers.begin(); it != schedulers.end(); it++)
	{
		cout << "	" << (*it)->timeSections.size() << " time sections" << endl;
	}
#endif // DEBUG
		
	for (int i = 0; i < dayLength; i++)
	{
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
		toChange->currentInterval = (toChange->currentInterval / 2) + 1;
}

pair<shared_ptr< vector<TimeSectionPtr>>, bool> SplitTimeSections(vector<TimeSectionPtr>& oldSections, const double upperBound, const double lowerBound) {
	vector<TimeSectionPtr> newSections;
	bool ready = true; //if any sections split or any merge it is set to false
	for (auto it = oldSections.begin(); it != oldSections.end(); it++)
	{
		
		if ((*it)->GetSectionLength() >= (*it)->currentInterval * 2 && !(*it)->merged) {
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
			ready = false;
		}
		else if ((*it)->potential < lowerBound && it + 1 != oldSections.end()) {//merge two 
			int time1 = (*it)->GetSectionLength();
			int time2 = (*(it + 1))->GetSectionLength();
			TimeSection section(time1 + time2, (*it)->currentInterval);
			section.merged = true;
			newSections.push_back(make_shared<TimeSection>(section));
			it++;
			ready = false;
		}
		else {
			TimeSection section((*it)->GetSectionLength(), (*it)->currentInterval);
			section.state = 0;		//anulating state of interval correction automat
			section.merged = (*it)->merged;
			newSections.push_back(make_shared<TimeSection>(section));
		}
	}
	return make_pair(make_shared< vector<TimeSectionPtr>>(newSections), ready);
}

void IntervalCorrectionAutomat(TimeSectionPtr toChange, const double upperBound, const double lowerBound) {
	switch (toChange->state) {
	case 0:
		toChange->cycle++;
		if (toChange->cycle >= accuracy) {
			toChange->state = 4;
		}
		else if (toChange->potential < lowerBound) {
			toChange->state = 1;
			IntervalCorrectionAutomat(toChange, upperBound, lowerBound);
		}
		else if (toChange->potential > upperBound) {
			toChange->state = -1;
			IntervalCorrectionAutomat(toChange, upperBound, lowerBound);
		}
		else {
			toChange->state = 3;
		}
		break;
	case 1: //increment
		if (toChange->potential < lowerBound) {
			if (toChange->currentInterval >= toChange->GetSectionLength())	//case when count of passengers is so low that even interval big as time section is not enough to pass to bound
				toChange->state = 4;
			else
				IncreaseInterval(toChange, lowerBound);		//state stays
		}
		else if (toChange->potential > upperBound) {	 //overflow so return to state 0
			toChange->state = 0;
			IntervalCorrectionAutomat(toChange, upperBound, lowerBound);
		}
		else {
			toChange->state = 3;
		}
		break;
	case -1: //decrement
		if (toChange->potential > upperBound) {
			if (toChange->currentInterval == 1)	//case when count of passengers is so big that even 1 minute interval cannot manage it
				toChange->state = 4;
			else
				DecreaseInterval(toChange);		//state stays
		}
		else if (toChange->potential < lowerBound) {	 //overflow so return to state 0
			toChange->state = 0;
			IntervalCorrectionAutomat(toChange, upperBound, lowerBound);
		}
		else {
			toChange->state = 3;
		}
		break;
	case 3:
		if (toChange->potential >= lowerBound && toChange->potential <= upperBound) {
			toChange->state = 4;
		}
		else {
			toChange->state = 0;
			IntervalCorrectionAutomat(toChange, upperBound, lowerBound);
		}
		break;
	}
	//...moved to function "AnulateScheduler" toChange->potential = 0;	
}

//upperBound and lowerBound are bounds of used potential of trains, they are between 0 and 1 according to percents
void CreateTimeTable(vector<SchedulerPtr>& schedulers, int dayLength /*in minutes*/, double upperBound, double lowerBound) {
	bool ready = false;
	while (!ready)
	{
		bool readyToSplit = false;
		while (!readyToSplit) {
			readyToSplit = true;		//while any interval is not in the state 4
			for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++) {
				(*sIt)->AnulateScheduler();
			}
			SimulateDay(schedulers, dayLength);
			
			for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++)//foreach scheduler ~ foreach line
			{
				for (auto tIt = (*sIt)->timeSections.begin(); tIt != (*sIt)->timeSections.end(); tIt++)	//foreach time section
				{
					if ((*tIt)->state != 4) {
						IntervalCorrectionAutomat((*tIt), upperBound, lowerBound);
						if ((*tIt)->state != 4)//if automat didn't change
							readyToSplit = false;
					}
				}
			}
		}

		ready = true; //if any time section is splitted or merged, ready is set to false;
		for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++)//foreach scheduler ~ foreach line
		{
			auto result = SplitTimeSections((*sIt)->timeSections, upperBound, lowerBound);
			auto newSections = *(result.first);
			if (!result.second)		//if any sections changed
				ready = false;
			(*sIt)->timeSections = newSections;
		}
		
		
	}
	for (auto sIt = schedulers.begin(); sIt != schedulers.end(); sIt++) {
		(*sIt)->AnulateScheduler();
	}
	SimulateDay(schedulers, dayLength);
}

int main(int argc, char** argv) {
	//number of arguments must be == 6 ~ name of program and 5 additional args
	if (argc != 6) {
		cout << "Invalid number of arguments";
		return 0;
	}
	//arg 1 ~ input file, arg 2 ~ lower potential bound, arg 3 ~ upper potential bound, arg 4 ~ accuary, arg 5 ~ train capacity
	try {
		inputFile = argv[1];
		outputFile = inputFile + ".out";
		lower = stod(argv[2]);
		upper = stod(argv[3]);
		accuracy = stoi(argv[4]);
		trainCapacity = stoi(argv[5]);
#ifdef DEBUG
		cout << "arguments" << endl;
		cout << "	" << argv[1] << endl;
		cout << "	" << argv[2] << endl;
		cout << "	" << argv[3] << endl;
		cout << "	" << argv[4] << endl;
		cout << "	" << argv[5] << endl;
#endif
	}
	catch (exception) {
		cout << "Arguments are wrong";
		return 0;
	}

	cout << "Entering parsing function." << endl << endl;
	Parser parser(inputFile);
	auto output = parser.ParseInputFile();
	auto subway = *(output.second);
	cout << "Input file was successfully parsed." << endl << endl;
	cout << "number of links is :" << subway.size() << endl;
	cout << "subway is working " << output.first << " hours per day" << endl << endl;

	
	vector<SchedulerPtr> schedulers;
	
	//creating schedulers
	for (auto it = subway.begin(); it != subway.end(); it++)
	{
		TimeSection section(output.first * 60, startingInterval);
		vector<TimeSectionPtr> sections;
		sections.push_back(make_shared<TimeSection>(section));
		Scheduler sch(output.first, (*it).second, sections);
		schedulers.push_back(make_shared<Scheduler>(sch));
	}

	cout << "Algorithm started..." << endl;
	//algorithm
	CreateTimeTable(schedulers, output.first * 60, upper, lower);
	cout << "Algorithm successfully ended." << endl;


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
	cout << "Timetable was created." << endl;



	return 0;
}

