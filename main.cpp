#include "global.hpp"

// Path to file contains todos
const std::string file_path = "~/.cache/doot_list.txt";

std::string str = R"(
[Doot]
School

[Dootling]
Math
"AP Stats":31231213:false
"AP Calculus AB":31231213:false

[Dootling]
English
"AP English Literature":31231213:false
"AP English Composition":31231213:false

[Doot]
School

[Dootling]
Math
"AP Stats":31231213:false
"AP Calculus AB":31231213:false

[Dootling]
English
"AP English Literature":31231213:false
"AP English Composition":31231213:false
)";

int main()
{
	// TODO: later add default names (Dooty #1, Dooty #2, etc.)
	Doots doots = parse_doots(str);

	// Print
	for (const Doot &doot : doots) {
		std::cout << doot.title << std::endl;
		for (const Dootling &dootling : doot.dootlings) {
			std::cout << dootling.title << std::endl;
			for (const SubDoot &subdoot : dootling.sub_doots) {
				std::cout << subdoot.task << ": " << subdoot.deadline << ": " << subdoot.done << std::endl;
			}
		}
	}
}
