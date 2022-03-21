#include "global.hpp"

////////////////////////////////////
// Static variables and functions //
////////////////////////////////////

const tuicpp::Table <SubDoot> ::Headers App::headers {
	"Task", "Deadline", "Status"
};

const tuicpp::Table <SubDoot> ::From App::from {
	App::headers, App::subdoot_element
};

std::string App::subdoot_element(const SubDoot &subdoot, size_t col)
{
	if (col == 0)
		return subdoot.task;
	else if (col == 1)
		return time_to_string(subdoot.deadline);
	else if (col == 2)
		return (subdoot.done ? "Done" : "Not Done");
	return "?";
}

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

	// Menu window
	menu = new tuicpp::DecoratedWindow("Options",
		tuicpp::ScreenInfo {
			.height = 10,
			.width = 25,
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
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);

	// Set table to some garbage
	auto f = from;
	subdoot_table = new tuicpp::Table <SubDoot> (
		f,
		tuicpp::ScreenInfo {
			.height = 10,
			.width = 58,
			.y = 24,
			.x = 1
		}
	);

	// TODO: get max x and y as a Window static method
	doots->set_keypad(true);
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

	// End ncurses
	endwin();
}

// Check inputs
void App::check_up()
{
	Dootlings &dts = doot_list[curr_doot].dootlings;
	switch (win_content) {
	case WIN_CONTENT_DOOTS:
		curr_doot = (curr_doot + doot_list.size() - 1)
			% doot_list.size();
		dootlings->erase();
		subdoot_table->erase();
		curr_subdoot = 0;
		break;
	case WIN_CONTENT_DOOTLINGS:
		curr_dootling = (curr_dootling + dts.size() - 1)
			% dts.size();
		subdoot_table->erase();
		curr_subdoot = 0;
		break;
	case WIN_CONTENT_SUBDOOTS:
		curr_subdoot = (curr_subdoot + dts[curr_dootling].subdoots.size() - 1)
			% dts[curr_dootling].subdoots.size();
		break;
	}
}

void App::check_down()
{
	Dootlings &dts = doot_list[curr_doot].dootlings;
	switch (win_content) {
	case WIN_CONTENT_DOOTS:
		curr_doot = (curr_doot + 1) % doot_list.size();
		dootlings->erase();
		subdoot_table->erase();
		curr_subdoot = 0;
		break;
	case WIN_CONTENT_DOOTLINGS:
		curr_dootling = (curr_dootling + 1) % dts.size();
		subdoot_table->erase();
		curr_subdoot = 0;
		break;
	case WIN_CONTENT_SUBDOOTS:
		curr_subdoot = (curr_subdoot + 1)
			% dts[curr_dootling].subdoots.size();
		break;
	}
}

void App::check_inputs(int c)
{
	// Switching active window
	if (c == KEY_LEFT)
		win_content = (win_content + 2) % 3;
	else if (c == KEY_RIGHT)
		win_content = (win_content + 1) % 3;

	// Switching active doot/dootling/subdoot
	if (c == KEY_UP)
		check_up();
	else if (c == KEY_DOWN)
		check_down();

	// Adding new doots, dootlings, or subdoots
	if (c == '=') {
		// doot_list.push_back(Doot {"New Doot", {}, false});
		int n;
		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			n = doot_list.size() + 1;
			doot_list.push_back(Doot {"Dooty " + std::to_string(n), {}, false});
			curr_doot = doot_list.size() - 1;
			create_editor();
			break;
		case WIN_CONTENT_DOOTLINGS:
			// doot_list[curr_doot].dootlings.push_back(Dootling());
			break;
		case WIN_CONTENT_SUBDOOTS:
			break;
		}
	}
}

// Create editor window
// TODO: pass type of fields to pass
// TODO: tuicpp class
void App::create_editor()
{
	auto pr = tuicpp::Window::limits();

	// TODO: static map of editor dimensions for each type of field
	int h = 10;
	int w = 20;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	editor = new tuicpp::DecoratedWindow("Editor",
		tuicpp::ScreenInfo {h, w, wy, wx}
	);
	editor->printf("title: ");

	// Allow cursor and echo
	curs_set(1);
	echo();

	auto tmp = tuicpp::FieldEditor <std::string, std::string> {
		"Editor",
		{"title", "title2"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = 1,
			.x = 1
		}
	};

	// TODO: dont return until done editing
	std::string title;
	int c;

	while ((c = editor->getc()) != 10) {
		if (c == KEY_BACKSPACE) {
			if (title.size() > 0)
				title.pop_back();
		} else {
			title += c;
		}

		editor->mvprintf(0, 7, "%s", title.c_str());
	}

	Doot &d = doot_list[curr_doot];
	d.title = title;

	// Reset current values
	curr_dootling = -1;
	curr_subdoot = -1;

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete subdoots;
	delete editor;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);

	// Reset cursor and echo
	curs_set(0);
	noecho();
}

// Run function
void App::run()
{
	// Main loop
	int c = 0;
	int line = 0;

	// TODO: selection box element and highlight box thingy

	do {
		// Check inputs
		check_inputs(c);

		// Highlight appropriate column
		// TODO: method
		doots->attr_title(A_NORMAL);
		dootlings->attr_title(A_NORMAL);
		subdoots->attr_title(A_NORMAL);

		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			doots->attr_title(A_REVERSE);
			break;
		case WIN_CONTENT_DOOTLINGS:
			dootlings->attr_title(A_REVERSE);
			break;
		case WIN_CONTENT_SUBDOOTS:
			subdoots->attr_title(A_REVERSE);
			break;
		}

		// TODO: method for options
		// TODO: tuicpp input handler (for compounding ie. + and
		// cntrl+shift, etc)
		menu->erase();
		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			menu->printf("[=]     Add doot\n");
			menu->printf("[-]     Remove doot\n");
			break;
		case WIN_CONTENT_DOOTLINGS:
			menu->printf("[=]     Add dootling\n");
			menu->printf("[-]     Remove dootling\n");
			break;
		case WIN_CONTENT_SUBDOOTS:
			menu->printf("[=]     Add subdoot\n");
			menu->printf("[-]     Remove subdoot\n");
			menu->printf("[Enter] Edit subdoot\n");
			break;
		}
		

		// TODO: method for printing
		// Print doots
		line = 0;
		for (size_t i = 0; i < doot_list.size(); i++) {
			if (i == curr_doot)
				doots->attribute_set(A_REVERSE);

			doots->mvprintf(line++, 0, doot_list[i].title.c_str());

			if (i == curr_doot)
				doots->attribute_set(A_NORMAL);
		}

		if (doot_list[curr_doot].dootlings.size() > 0) {
			// Print dootling for selected doot
			Dootlings ds = doot_list[curr_doot].dootlings;

			line = 0;
			for (size_t i = 0; i < ds.size(); i++) {
				if (i == curr_dootling)
					dootlings->attribute_set(A_REVERSE);

				dootlings->mvprintf(line++, 0, ds[i].title.c_str());

				if (i == curr_dootling)
					dootlings->attribute_set(A_NORMAL);
			}

			if (ds[curr_dootling].subdoots.size() > 0) {
				// Update subdoot table
				SubDoots sd = ds[curr_dootling].subdoots;
				subdoot_table->set_lengths({20, 20, 8});
				subdoot_table->set_data(sd);
				subdoot_table->highlight_row(curr_subdoot);
			}
		}

		// Refresh windows
		subdoot_table->refresh();
		dootlings->refresh();
		doots->refresh();
	} while ((c = doots->getc()) != 'q');
}
