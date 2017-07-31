#include <string>
#include <iostream>
#include "LootSort.h"

static void report_progress(const std::string& msg)
{
	std::cerr << msg << '\n';
}

int main(int argc, char** argv)
{
	if (argc <= 1)
		return(sort_auto_loot(std::string("Loot.ini"), report_progress));
	for (auto index = 1; index < argc; ++index)
	{
		auto error = sort_auto_loot(std::string(argv[index]), report_progress);
		if (error) return error;
	}
	return 0;
}