#include <string>
#include <iostream>
#include "LootSort.h"

int main()
{
	return(sort_auto_loot(std::string("Loot.ini"),
							[](auto msg) {std::cerr << msg << '\n'; }));
}