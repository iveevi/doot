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

// Convert unix time to human readable time
std::string time_to_string(int time)
{
	// Conver to time_t
	time_t t = time;

	// Convert to string
	std::string s = ctime(&t);

	// Remove newline
	s.pop_back();

	return s;
}

/////////////////////////
// Write todos to file //
/////////////////////////

void write_subdoot(std::ostream &fout, const SubDoot &subdoot)
{
	fout << "\"" << subdoot.task << "\"" << ":" << subdoot.deadline
		<< ":" << std::boolalpha << subdoot.done << "\n";
}

void write_dootling(std::ostream &fout, const Dootling &dootling)
{
	fout << "[Dootling]\n" << dootling.title << "\n";

	for (const auto &subdoot : dootling.subdoots)
		write_subdoot(fout, subdoot);
}

void write_doot(std::ostream &fout, const Doot &doot)
{
	fout << "[Doot]\n" << doot.title << "\n";

	for (const auto &dootling : doot.dootlings)
		write_dootling(fout, dootling);
}

// TODO: Table class
// to update:
// - either a list of vectors
// - or list of structs, then lambda to process

int main()
{
	// TODO: later add default names (Dooty #1, Dooty #2, etc.)
	Doots doots = parse_doots(str);

	// Initialize ncurses
	initscr();
        noecho();
        curs_set(0);

	// Initialize colors
        start_color();

	// Highlight color
	init_pair(1, COLOR_WHITE, COLOR_RED);

	// TODO: wrap all this in a class (BoxedWindow derivative)

	// Menu window
	tuicpp::BoxedWindow menu("Menu",
		tuicpp::ScreenInfo {
			.height = 20,
			.width = 20,
			.y = 40,
			.x = 0
		}
	);

	int height = tuicpp::BoxedWindow::decoration_height
		+ std::max(doots.size(), 10ul);
	tuicpp::BoxedWindow win("Doots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 0
		}
	);

	tuicpp::BoxedWindow dootlings("Dootlings",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 20
		}
	);
	
	tuicpp::BoxedWindow subdoots("Subdoots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);

	win.set_keypad(true);

	int c = 0;
	
	int highlight = 0;
	int column = 0;

	int line = 0;
	int dootling = -1;

	// TODO: selection box element and highlight box thingy

	do {
		// Check up and down keys
		// TODO: function
		if (c == KEY_LEFT) {
			column = 0;
			dootling = 0;
		} else if (c == KEY_RIGHT) {
			column = 1;
			dootling = 0;
		}

		if (column == 0) {
			if (c == KEY_UP) {
				highlight--;
				if (highlight < 0)
					highlight = doots.size() - 1;
				else
					dootling = -1;
			} else if (c == KEY_DOWN) {
				highlight++;
				if (highlight >= doots.size())
					highlight = 0;
				else
					dootling = -1;
			}
		} else {
			if (c == KEY_UP) {
				if (dootling == -1)
					dootling = doots[highlight].dootlings.size() - 1;
				else
					dootling--;
				if (dootling < 0)
					dootling = doots[highlight].dootlings.size() - 1;
			} else if (c == KEY_DOWN) {
				dootling++;
				if (dootling >= doots[highlight].dootlings.size())
					dootling = 0;
			}
		}

		// Print doots
		line = 0;
		for (size_t i = 0; i < doots.size(); i++) {
			if (i == highlight)
				win.attribute_on(COLOR_PAIR(1));

			win.mvprintf(line++, 0, doots[i].title.c_str());

			if (i == highlight)
				win.attribute_off(COLOR_PAIR(1));
		}

		// Print dootling for selected doot
		Dootlings ds = doots[highlight].dootlings;

		line = 0;
		for (size_t i = 0; i < ds.size(); i++) {
			if (i == dootling)
				dootlings.attribute_on(COLOR_PAIR(1));

			dootlings.mvprintf(line++, 0, ds[i].title.c_str());

			if (i == dootling)
				dootlings.attribute_off(COLOR_PAIR(1));
		}

		if (dootling >= 0) {
			SubDoots sd = ds[dootling].subdoots;

			line = 0;
			for (size_t i = 0; i < sd.size(); i++)
				subdoots.mvprintf(line++, 0, sd[i].task.c_str());
		}

		// Refresh windows
		dootlings.refresh();
		win.refresh();
	} while ((c = win.getc()) != 'q');

	endwin();

	// Check if file_path exists
	

	// Open file
	std::ofstream fout(file_path);

	if (!fout.is_open()) {
		std::cerr << "Could not open file " << file_path << std::endl;
		return 1;
	}

	// Write doots
	std::cout << "Writing doots to " << file_path << std::endl;
	for (const auto &doot : doots)
		write_doot(fout, doot);

	// Close file
	fout.close();

	return 0;
}
