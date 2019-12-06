#include <istream>
#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Original author leveraged rename from the experimental filesystem lib. This lib is no longer experimental in C++17
// and VS2019 will throw an error unless the below deprecation warning is defined. Must be defined prior to the #include.
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>


#include "LootSort.h"

typedef std::vector<std::string> Section;

static void read_section(Section& section, std::istream& is)
{
	section.clear();
	std::string line;
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

static void sort_auto_loot(std::ostream& os, std::istream& is)
{
	while (!is.eof()) {
		Section section;
		read_section(section, is);
		sort_section_if_needed(section);
		for (const std::string& line : section) {
			os << line << '\n';
			if (os.bad()) return;
		}
	}
}

static bool is_lootfile(std::istream& is)
{
	std::string first_line;
	is.seekg(0, is.beg);
	std::getline(is, first_line);
	is.seekg(0, is.beg);
	return (!is.bad() && first_line == "[Settings]");
}

/// <summary>Sort the loot sections in the MQ2AutoLoot ini file</summary>
/// <param name="lootfile">Name of the MQ2AutoLoot ini file to be sorted</param>
/// <param name="report">Pointer to a function that will be called for reporting. Maybe nullptr.</param>
/// <returns>0: OK, 1: can't open lootfile, 2: can't open tempfile, 3: problem with tempfile during sort</returns>
int sort_auto_loot(const std::string& lootfile, void(*report)(const std::string&))
{
	std::ifstream is;
	is.open(lootfile);
	if (!is.is_open()) {
		if (report) report("Can't open input: " + lootfile);
		return (1);
	}
	if (!is_lootfile(is)) {
		if (report) report("This isn't a lootfile: " + lootfile);
		return (2);
	}
	const std::string tempfile(lootfile + ".sav");
	std::ofstream os;
	os.open(tempfile, std::ios_base::out | std::ios_base::trunc);
	if (!os.is_open()) {
		if (report) report("Can't open output: " + tempfile);
		return (3);
	}
	sort_auto_loot(os, is);
	if (os.bad()) {
		if (report) report("Sort failed");
		return (4);
	}
	is.close();
	os.close();
	const std::string tempfile2(lootfile + ".sav2");
	

	// Pre-C++17: experimental::filesystem::rename(const path& old_p, const path& new_p)
	// Post-C++17: filesystem::rename(const std::filesystem::path& old_p, const std::filesystem::path& new_p, std::error_code& ec) noexcept;
	// Post-C++17 _should_ implicitely convert std::string to paths.
	
	// swap tempfile, lootfile
	std::experimental::filesystem::rename(tempfile, tempfile2);
	std::experimental::filesystem::rename(lootfile, tempfile);
	std::experimental::filesystem::rename(tempfile2, lootfile);
	if (report) report("Sorted: " + lootfile);
	return(0);
}