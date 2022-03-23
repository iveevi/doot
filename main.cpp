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

int main()
{
	// TODO: -d option for daemon mode
	std::cout << "Daemon mode? (y/n): ";
	std::string input;
	std::cin >> input;

	if (input == "y") {
		std::cout << "Daemon mode enabled" << std::endl;

		Doots doots = parse_doots(str);
		while (true) {
			// Get current time
			long int now = time(NULL);

			// Check if any subdoots are overdue
			for (auto &doot : doots) {
				for (auto &dootling : doot.dootlings) {
					for (auto &subdoot : dootling.subdoots) {
						if (subdoot.deadline < now
								&& !subdoot.done
								&& !subdoot.overdued) {
							// TODO: need to mark as
							// notified
							std::string msg = "[" + doot.title + "/"
								+ dootling.title + "] Overdue task "
								+ subdoot.task;
							notify(msg);
							subdoot.overdued = true;
						}
					}
				}
			}

			// TODO: sleep and reload doots
			// sleep(60);
		}
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
