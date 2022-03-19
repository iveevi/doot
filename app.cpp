#include "global.hpp"

///////////////////////
// Application class //
///////////////////////

// Constructor
App::App(const Doots &dts) : doot_list(dts)
{
	// Initialize ncurses
	initscr();
        noecho();
        curs_set(0);

	// Initialize colors
        start_color();

	// Highlight color
	init_pair(1, COLOR_WHITE, COLOR_RED);

	// Menu window
	menu = new tuicpp::DecoratedWindow("Menu",
		tuicpp::ScreenInfo {
			.height = 6,
			.width = 20,
			.y = 40,
			.x = 0
		}
	);

	// Doots
	int height = tuicpp::DecoratedWindow::decoration_height
		+ std::max(doot_list.size(), 10ul);

	doots = new tuicpp::DecoratedWindow("Doots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 0
		}
	);

	// Dootlings window
	dootlings = new tuicpp::DecoratedWindow("Dootlings",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 20,
			.y = 0,
			.x = 20
		}
	);

	// Subdoots window
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = height,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);

	// Table window
	Dootling dtng = doot_list[0].dootlings[0];

	subdoot_table = new tuicpp::Table <SubDoot> (
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

	// Editor window at the bottom
	int max_width = 0;
	int max_height = 0;

	getmaxyx(stdscr, max_height, max_width);
	editor = new tuicpp::BoxedWindow(
		tuicpp::ScreenInfo {
			.height = 3,
			.width = max_width,
			.y = max_height - 3,
			.x = 0
		}
	);
	
	editor->set_keypad(true);
}

// Destructor
App::~App()
{
	// Clean up
	delete doots;
	delete dootlings;
	delete subdoots;
	delete subdoot_table;
	delete menu;
	delete editor;

	// End ncurses
	endwin();
}

// Run function
void App::run()
{
	// Main loop
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

		// Highlight appropriate column
		if (column == 0) {
			doots->attr_title(A_REVERSE);
			dootlings->attr_title(A_NORMAL);

			menu->erase();
			menu->printf("[+] Add doot");
		} else {
			doots->attr_title(A_NORMAL);
			dootlings->attr_title(A_REVERSE);

			menu->erase();
			menu->printf("[+] Add dootling");
		}

		if (column == 0) {
			if (c == KEY_UP) {

				highlight--;
				if (highlight < 0)
					highlight = doot_list.size() - 1;
				else
					dootling = -1;
				
				dootlings->erase();
			} else if (c == KEY_DOWN) {
				highlight++;
				if (highlight >= doot_list.size())
					highlight = 0;
				else
					dootling = -1;
				
				dootlings->erase();
			}
		} else {
			if (c == KEY_UP) {
				if (dootling == -1)
					dootling = doot_list[highlight].dootlings.size() - 1;
				else
					dootling--;
				if (dootling < 0)
					dootling = doot_list[highlight].dootlings.size() - 1;

				dootlings->erase();
			} else if (c == KEY_DOWN) {
				dootling++;
				if (dootling >= doot_list[highlight].dootlings.size())
					dootling = 0;
				
				dootlings->erase();
			}
		}

		// Print doots
		line = 0;
		for (size_t i = 0; i < doot_list.size(); i++) {
			if (i == highlight)
				doots->attribute_on(COLOR_PAIR(1));

			doots->mvprintf(line++, 0, doot_list[i].title.c_str());

			if (i == highlight)
				doots->attribute_off(COLOR_PAIR(1));
		}

		// Print dootling for selected doot
		Dootlings ds = doot_list[highlight].dootlings;

		line = 0;
		for (size_t i = 0; i < ds.size(); i++) {
			if (i == dootling)
				dootlings->attribute_on(COLOR_PAIR(1));

			dootlings->mvprintf(line++, 0, ds[i].title.c_str());

			if (i == dootling)
				dootlings->attribute_off(COLOR_PAIR(1));
		}

		if (dootling >= 0) {
			SubDoots sd = ds[dootling].subdoots;

			line = 0;
			for (size_t i = 0; i < sd.size(); i++)
				subdoots->mvprintf(line++, 0, sd[i].task.c_str());
		}

		// Refresh windows
		subdoots->refresh();
		dootlings->refresh();
		doots->refresh();
	} while ((c = editor->getc()) != 'q');
}
