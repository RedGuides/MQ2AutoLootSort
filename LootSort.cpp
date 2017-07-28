#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "LootSort.h"

using namespace std;

typedef vector<string> Section;

static void read_section(Section& section, istream& is)
{
	section.clear();
	string line;
	while (getline(is, line)) {		
		section.push_back(line);
		if (is.peek() == '[') break; // is next line a new section
	}
}

static void sort_section_if_needed(Section& section)
{
	if (section.size() < 2) return;
	// check for 1 char section name
	if (section[0].length() < 3) return;
	if (section[0][0] != '[') return;
	if (section[0][2] != ']') return;
	auto begin = section.begin() + 1;
	if (begin[0][0] == '|' || begin[0][0] == ';') ++begin; // don't sort comment line
	sort(begin, section.end());
}

static void sort_auto_loot(ostream& os, istream& is)
{
	while (!is.eof()) {
		Section section;
		read_section(section, is);
		sort_section_if_needed(section);
		for (const string& line : section) {
			os << line << '\n';
			if (os.bad()) return;
		}
	}
}

/// <summary>Sort the loot sections in the MQ2AutoLoot ini file</summary>
/// <param name="lootfile">Name of the MQ2AutoLoot ini file to be sorted</param>
/// <param name="report">Pointer to a function that will be called for reporting. Maybe nullptr.</param>
/// <returns>0: OK, 1: can't open lootfile, 2: can't open tempfile, 3: problem with tempfile during sort</returns>
int sort_auto_loot(const string& lootfile, void(*report)(const string&))
{
	const string tempfile(lootfile + ".sav");
	ifstream is;
	is.open(lootfile);
	if (!is.is_open()) {
		if (report) report("Can't open input: " + lootfile);
		return (1);
	}
	ofstream os;
	os.open(tempfile, ios_base::out | ios_base::trunc);
	if (!os.is_open()) {
		if (report) report("Can't open output: " + tempfile);
		return (2);
	}
	sort_auto_loot(os, is);
	if (os.bad()) {
		if (report) report("Sort failed");
		return (3);
	}
	is.close();
	os.close();
	const string tempfile2(lootfile + ".sav2");
	// swap tempfile, lootfile
	using namespace std::experimental::filesystem;
	rename(tempfile, tempfile2);
	rename(lootfile, tempfile);
	rename(tempfile2, lootfile);
	if (report) report("Sorted: " + lootfile);
	return(0);
}