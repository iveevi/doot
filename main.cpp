#include "global.hpp"

// Path to file contains todos
// TODO: add a cli option to set file in case of multiple todos
const std::string file_path = "doot_list.txt";

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
xSchool

[Dootling]
xMath
"xAP Stats":31231213:false
"xAP Calculus AB":31231213:false

[Dootling]
xEnglish
"xAP English Literature":31231213:false
"xAP English Composition":31231213:false
)";

// TODO: Table class
// to update:
// - either a list of vectors
// - or list of structs, then lambda to process

int main()
{
	// TODO: later add default names (Dooty #1, Dooty #2, etc.)
	Doots doots = parse_doots(str);

	App app(doots);
	app.run();

	return 0;
}
