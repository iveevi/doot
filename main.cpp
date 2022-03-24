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

void notify(const std::string &msg)
{
	static std::string ncmd = "notify-send -t 5000 'doot' ";
	static std::string pcmd = "aplay";

	std::string cmd = ncmd + " \'" + msg + "\' "
		+ " && " + pcmd + " sounds/notif_sound_1.wav";

	system(cmd.c_str());
}

// Get subdoots from doots
struct ExtendedSubDoot {
	std::string doot;
	std::string dootling;

	SubDoot subdoot;
};

using ExtendedSubDoots = std::vector <ExtendedSubDoot>;

ExtendedSubDoots get_esubdoots(const Doots &doots)
{
	ExtendedSubDoots esubdoots;

	for (const auto &doot : doots) {
		for (const auto &dootling : doot.dootlings) {
			for (const auto &subdoot : dootling.subdoots) {
				esubdoots.push_back({
					doot.title,
					dootling.title,
					subdoot
				});
			}
		}
	}

	return esubdoots;
}

// Daemon program
void daemon()
{
	std::set <std::string> marked_overdue;

	while (true) {
		std::string str = read_glob(file_path);
		Doots doots = parse_doots(str);

		// Get current time
		long int now = time(NULL);

		ExtendedSubDoots esubdoots = get_esubdoots(doots);
		for (auto &esubdoot : esubdoots) {
			SubDoot subdoot = esubdoot.subdoot;
			std::string name = subdoot.task;
			if (subdoot.deadline < now && !subdoot.done
					&& !marked_overdue.count(name)) {
				std::string msg = "[" + esubdoot.doot + "/"
					+ esubdoot.dootling + "] Overdue task "
					+ name;

				std::cout << msg << std::endl;
				notify(msg);

				// Mark as overdue
				marked_overdue.insert(name);
			}
		}

		// TODO: delay should be configurable
		sleep(1);
	}
}

int main()
{
	// TODO: -d option for daemon mode
	std::cout << "Daemon mode? (y/n): ";
	std::string input;
	std::cin >> input;

	if (input == "y") {
		std::cout << "Daemon mode enabled" << std::endl;
		daemon();
	} else {
		std::string str = read_glob("doot_list.txt");
		Doots doots = parse_doots(str);

		App app(doots);
		app.run();

		// Save doots
		app.save("doot_list.txt");
	}

	return 0;
}
