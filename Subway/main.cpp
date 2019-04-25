#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include "simulation.h"
#include "inputParser.h"

using namespace std;


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


int main() {
	Parser parser("input.txt");
	auto output = parser.ParseInputFile();
	auto subway = output.second;
	cout << "number of links is :" << subway.size() << endl;
	cout << "subway is working " << output.first << " hours per day" << endl;
	
	vector<SchedulerPtr> schedulers;
	TimeSection section(output.first * 60, 10);
	vector<TimeSectionPtr> sections;
	sections.push_back(make_shared<TimeSection>(section));
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

