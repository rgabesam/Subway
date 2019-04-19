#include <sstream>
#include <iostream>
#include <string>
#include <map>

#include "simulation.h"
#include "inputParser.h"

using namespace std;

int main() {
	Parser parser("input.txt");
	auto subway = parser.ParseInputFile();

	cout << subway.size();

	//string s;
	//cin >> s;
	return 0;
}