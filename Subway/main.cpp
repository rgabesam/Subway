#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include "simulation.h"
#include "inputParser.h"

using namespace std;

int main() {
	Parser parser("input.txt");
	auto output = parser.ParseInputFile();
	auto subway = output.second;
	cout << "number of links is :" << subway.size() << endl;
	cout << "subway is working " << output.first << " hours per day";

	//string s;
	//cin >> s;
	return 0;
}