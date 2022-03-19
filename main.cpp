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
	tuicpp::DecoratedWindow menu("Menu",
		tuicpp::ScreenInfo {
			.height = 6,
			.width = 20,
			.y = 40,
			.x = 0
		}
	);

	tuicpp::PlainWindow bwin(
		tuicpp::ScreenInfo {
			.height = 6,
			.width = 60,
			.y = 40,
			.x = 20
		}
	);

	// Table on bwin using first dootling
	Dootling dtng = doots[0].dootlings[0];

	// Table window
	tuicpp::Table <SubDoot> table(
		{"Task", "Deadline", "Status"},
		dtng.subdoots,
		[&](const SubDoot &subdoot, size_t col) -> std::string {
			if (col == 0)
				return subdoot.task;
			else if (col == 1)
				return time_to_string(subdoot.deadline);
			else if (col == 2)
				return (subdoot.done ? "Done" : "Not Done");
			else
				return "?";
		},
		tuicpp::ScreenInfo {
			.height = 10,
			.width = 60,
			.y = 50,
			.x = 0
		}
	);
	table.refresh();

	std::vector <std::string> columns {
		"Task",
		"Deadline",
		"Status"
	};

	// Longest lengths for each column
	std::vector <int> lengths {
		0,
		0,
		0
	};

	for (const auto &subdoot : dtng.subdoots) {
		std::string t = time_to_string(subdoot.deadline);
		std::string s = subdoot.done ? "Done" : "Not done";
		lengths[0] = std::max(lengths[0], (int) subdoot.task.size());
		lengths[1] = std::max(lengths[1], (int) t.size());
		lengths[2] = std::max(lengths[2], (int) s.size());
	}

	int total_width = 0;
	for (const auto &length : lengths)
		total_width += length + 1;

	int line = 0;

	// Top bar
	int x = 0;
	
	bwin.mvadd_char(line, 0, ACS_ULCORNER);
	for (size_t i = 0; i < columns.size(); i++) {
		for (int j = 0; j < lengths[i] + 2; j++)
			bwin.mvadd_char(line, x + j + 1, ACS_HLINE);
		x += lengths[i] + 3;

		if (i != columns.size() - 1)
			bwin.mvadd_char(line, x, ACS_TTEE);
		else
			bwin.mvadd_char(line, x, ACS_URCORNER);
	}
	line++;

	// Columns
	x = 1;
	for (size_t i = 0; i < columns.size(); i++) {
		bwin.mvprintf(line, x, " %s ", columns[i].c_str());
		x += lengths[i] + 3;
		bwin.mvadd_char(line, x - 1, ACS_VLINE);
	}
	bwin.mvadd_char(line, 0, ACS_VLINE);
	line++;

	// Middle bar
	x = 0;
	
	bwin.mvadd_char(line, 0, ACS_LTEE);
	for (size_t i = 0; i < columns.size(); i++) {
		for (int j = 0; j < lengths[i] + 2; j++)
			bwin.mvadd_char(line, x + j + 1, ACS_HLINE);
		x += lengths[i] + 3;

		if (i != columns.size() - 1)
			bwin.mvadd_char(line, x, ACS_PLUS);
		else
			bwin.mvadd_char(line, x, ACS_RTEE);
	}
	line++;

	// Rows
	for (const auto &subdoot : dtng.subdoots) {
		int x = 0;
		bwin.mvadd_char(line, x, ACS_VLINE); x++;

		std::string t = time_to_string(subdoot.deadline);
		std::string s = subdoot.done ? "Done" : "Not done";

		bwin.mvprintf(line, x, " %s ", subdoot.task.c_str());  x += lengths[0] + 2;
		bwin.mvadd_char(line, x, ACS_VLINE); x++;
		
		bwin.mvprintf(line, x, " %s ", t.c_str());  x += lengths[1] + 2;
		bwin.mvadd_char(line, x, ACS_VLINE); x++;
		
		bwin.mvprintf(line, x, " %s ", s.c_str()); x += lengths[2] + 2;
		bwin.mvadd_char(line, x, ACS_VLINE); 
		line++;
	}

	// Bottom bar
	x = 0;
	
	bwin.mvadd_char(line, 0, ACS_LLCORNER);
	for (size_t i = 0; i < columns.size(); i++) {
		for (int j = 0; j < lengths[i] + 2; j++)
			bwin.mvadd_char(line, x + j + 1, ACS_HLINE);
		x += lengths[i] + 3;

		if (i != columns.size() - 1)
			bwin.mvadd_char(line, x, ACS_BTEE);
		else
			bwin.mvadd_char(line, x, ACS_LRCORNER);
	}
	line++;

	bwin.refresh();

	int height = tuicpp::DecoratedWindow::decoration_height
		+ std::max(doots.size(), 10ul);
	tuicpp::DecoratedWindow win("Doots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 0
		}
	);

	tuicpp::DecoratedWindow dootlings("Dootlings",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 20
		}
	);
	
	tuicpp::DecoratedWindow subdoots("Subdoots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);

	// Editor window at the bottom
	int max_width = 0;
	int max_height = 0;

	getmaxyx(stdscr, max_height, max_width);
	tuicpp::BoxedWindow editor(
		tuicpp::ScreenInfo {
			.height = 3,
			.width = max_width,
			.y = max_height - 3,
			.x = 0
		}
	);

	win.set_keypad(true);

	int c = 0;
	
	int highlight = 0;
	int column = 0;

	line = 0;
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

		// Highlight appropriate column
		if (column == 0) {
			win.attr_title(A_REVERSE);
			dootlings.attr_title(A_NORMAL);

			menu.erase();
			menu.printf("[+] Add doot");
		} else {
			win.attr_title(A_NORMAL);
			dootlings.attr_title(A_REVERSE);

			menu.erase();
			menu.printf("[+] Add dootling");
		}

		if (column == 0) {
			if (c == KEY_UP) {

				highlight--;
				if (highlight < 0)
					highlight = doots.size() - 1;
				else
					dootling = -1;
				
				dootlings.erase();
			} else if (c == KEY_DOWN) {
				highlight++;
				if (highlight >= doots.size())
					highlight = 0;
				else
					dootling = -1;
				
				dootlings.erase();
			}
		} else {
			if (c == KEY_UP) {
				if (dootling == -1)
					dootling = doots[highlight].dootlings.size() - 1;
				else
					dootling--;
				if (dootling < 0)
					dootling = doots[highlight].dootlings.size() - 1;

				dootlings.erase();
			} else if (c == KEY_DOWN) {
				dootling++;
				if (dootling >= doots[highlight].dootlings.size())
					dootling = 0;
				
				dootlings.erase();
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
