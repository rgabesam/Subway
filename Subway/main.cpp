#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include "simulation.h"
#include "inputParser.h"

using namespace std;

const int startingInterval = 5;


void SimulateDay(vector<SchedulerPtr> schedulers, int dayLength /*in minutes*/) {
	auto schedulersIt = schedulers.begin();
	for (int i = 0; i < dayLength; i++)
	{
		cout << "Simulating minute number " << i + 1 << " of " << dayLength << endl;
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
void ChangeIntervals(TimeSectionPtr toChange, const int mode, const bool increase) {
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
}



int main() {
	Parser parser("input.txt");
	auto output = parser.ParseInputFile();
	auto subway = output.second;
	cout << "number of links is :" << subway.size() << endl;
	cout << "subway is working " << output.first << " hours per day" << endl;
	
	vector<SchedulerPtr> schedulers;
	//TimeSection section(output.first * 30, 1);
	TimeSection section(output.first * 60, startingInterval);
	vector<TimeSectionPtr> sections;
	sections.push_back(make_shared<TimeSection>(section));
	//section = TimeSection(output.first * 30, 2);
	//sections.push_back(make_shared<TimeSection>(section));
	for (auto it = subway.begin(); it != subway.end(); it++)
	{
		Scheduler sch(output.first, (*it).second, sections);
		schedulers.push_back(make_shared<Scheduler>(sch));
	}
	SimulateDay(schedulers, output.first * 60);
	//string s;
	//cin >> s;
	return 0;
}

