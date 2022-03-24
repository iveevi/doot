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
			.width = 30,
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

	// TODO: help window, list supported date formats

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
		int n;
		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			create_doot_editor();
			break;
		case WIN_CONTENT_DOOTLINGS:
			create_dootling_editor();
			break;
		case WIN_CONTENT_SUBDOOTS:
			create_subdoot_editor();
			break;
		}
	}

	// Deleting doots, dootlings, or subdoots
	if (c == '-') {
		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			delete_doot();
			break;
		case WIN_CONTENT_DOOTLINGS:
			delete_dootling();
			break;
		case WIN_CONTENT_SUBDOOTS:
			delete_subdoot();
			break;
		}
	}

	// Editing doots, dootlings, or subdoots
	if (c == 'e') {
		switch (win_content) {
		case WIN_CONTENT_DOOTS:
			edit_doot();
			break;
		case WIN_CONTENT_DOOTLINGS:
			edit_dootling();
			break;
		case WIN_CONTENT_SUBDOOTS:
			edit_subdoot();
			break;
		}
	}
}

// Create doot editor window
void App::create_doot_editor()
{
	auto pr = tuicpp::Window::limits();

	int h = 7;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"title"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	std::string title;
	bool filled = editor->yield({
		tuicpp::yielder(&title)
	});

	// Save values
	if (filled) {
		doot_list.push_back(Doot {title, {}, false});
		curr_doot = doot_list.size() - 1;
	}

	// Reset current values
	curr_dootling = 0;
	curr_subdoot = 0;

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);
}

// Create dootling editor window
void App::create_dootling_editor()
{
	auto pr = tuicpp::Window::limits();

	int h = 7;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"title"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	std::string title;
	bool filled = editor->yield({
		tuicpp::yielder(&title)
	});

	// Save values
	if (filled) {
		doot_list[curr_doot].dootlings.push_back(
			Dootling {title, {}, false}
		);

		curr_dootling = doot_list[curr_doot].dootlings.size() - 1;

		// Reset current values
		curr_subdoot = 0;
	}

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);
}

// Create subdoot editor window
void App::create_subdoot_editor()
{
	auto pr = tuicpp::Window::limits();

	int h = 9;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	// TODO: make a "move_update" parameter to update
	// the editor window when cursor moved
	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"task", "date", "time"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	std::string title;
	std::string deadline;
	std::string time;

	bool filled = editor->yield({
		tuicpp::yielder(&title),
		tuicpp::yielder(&deadline),
		tuicpp::yielder(&time)
	});

	// Save values
	// TODO: function here
	if (filled) {
		StringFeeder sf_deadline(deadline);
		StringFeeder sf_time(time);

		auto ret1 = rule <date_time> ::value(&sf_time);
		auto ret2 = rule <date> ::value(&sf_deadline);

		date_time dt = get <date_time> (ret1);
		date d = get <date> (ret2);

		std::tm tm = {
			.tm_sec = dt.second,
			.tm_min = dt.minute,
			.tm_hour = dt.hour,
			.tm_mday = d.day,
			.tm_mon = d.month - 1,
			.tm_year = d.year - 1900,
			.tm_isdst = -1
		};

		time_t t = mktime(&tm);

		doot_list[curr_doot].dootlings[curr_dootling].subdoots.push_back(
			SubDoot {
				title,
				t,
				false
			}
		);

		curr_subdoot = doot_list[curr_doot]
			.dootlings[curr_dootling]
			.subdoots.size() - 1;
	}

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);
}

// Delete current doot
void App::delete_doot()
{
	doot_list.erase(doot_list.begin() + curr_doot);
	curr_doot = std::max(0, curr_doot - 1);
	curr_dootling = 0;
	curr_subdoot = 0;

	doots->erase();
	dootlings->erase();
	subdoot_table->erase();
}

// Delete current dootling
void App::delete_dootling()
{
	doot_list[curr_doot].dootlings.erase(
		doot_list[curr_doot].dootlings.begin()
			+ curr_dootling
	);

	curr_dootling = std::max(0, curr_dootling - 1);
	curr_subdoot = 0;

	dootlings->erase();
	subdoot_table->erase();
}

// Delete current subdoot
void App::delete_subdoot()
{
	doot_list[curr_doot]
		.dootlings[curr_dootling]
		.subdoots.erase(
			doot_list[curr_doot]
				.dootlings[curr_dootling]
				.subdoots.begin() + curr_subdoot
		);

	curr_subdoot = std::max(0, curr_subdoot - 1);

	dootlings->erase();
	subdoot_table->erase();
}

// Edit current doot
// TODO: some easier system for all this window creation
void App::edit_doot()
{
	auto pr = tuicpp::Window::limits();

	int h = 7;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"title"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	bool filled = editor->yield({
		tuicpp::yielder(&doot_list[curr_doot].title)
	});

	// Reset current values
	curr_dootling = 0;
	curr_subdoot = 0;

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);
}

// Edit current dootling
void App::edit_dootling()
{
	auto pr = tuicpp::Window::limits();

	int h = 7;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"title"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	bool filled = editor->yield({
		tuicpp::yielder(&doot_list[curr_doot]
			.dootlings[curr_dootling].title
		)
	});

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	);
}

// Create subdoot editor window
void App::edit_subdoot()
{
	// TODO: fill out
	/* auto pr = tuicpp::Window::limits();

	int h = 9;
	int w = 30;

	int wy = (pr.first - h)/2;
	int wx = (pr.second - w)/2;

	// TODO: make a "move_update" parameter to update
	// the editor window when cursor moved
	auto editor = new tuicpp::FieldEditor {
		"Editor",
		{"task", "date", "time"},
		tuicpp::ScreenInfo {
			.height = h,
			.width = w,
			.y = wy,
			.x = wx
		}
	};

	std::string title;
	std::string deadline;
	std::string time;

	bool filled = editor->yield({
		tuicpp::yielder(&title),
		tuicpp::yielder(&deadline),
		tuicpp::yielder(&time)
	});

	// Save values
	// TODO: function here
	if (filled) {
		StringFeeder sf_deadline(deadline);
		StringFeeder sf_time(time);

		auto ret1 = rule <date_time> ::value(&sf_time);
		auto ret2 = rule <date> ::value(&sf_deadline);

		date_time dt = get <date_time> (ret1);
		date d = get <date> (ret2);

		std::tm tm = {
			.tm_sec = dt.second,
			.tm_min = dt.minute,
			.tm_hour = dt.hour,
			.tm_mday = d.day,
			.tm_mon = d.month - 1,
			.tm_year = d.year - 1900,
			.tm_isdst = -1
		};

		time_t t = mktime(&tm);

		doot_list[curr_doot].dootlings[curr_dootling].subdoots.push_back(
			SubDoot {
				title,
				t,
				false,
				false
			}
		);

		curr_subdoot = doot_list[curr_doot]
			.dootlings[curr_dootling]
			.subdoots.size() - 1;
	}

	dootlings->erase();
	subdoot_table->erase();

	// Remove the editor and remake subdoots 
	delete editor;
	delete subdoots;

	// TODO: function to clear and function to reinitialize ALL windows
	subdoots = new tuicpp::DecoratedWindow("Subdoots",
		tuicpp::ScreenInfo {
			.height = 15,
			.width = 60,
			.y = 20,
			.x = 0
		}
	); */
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
			menu->printf("[e]     Edit doot\n");
			break;
		case WIN_CONTENT_DOOTLINGS:
			menu->printf("[=]     Add dootling\n");
			menu->printf("[-]     Remove dootling\n");
			menu->printf("[e]     Edit dootling\n");
			break;
		case WIN_CONTENT_SUBDOOTS:
			menu->printf("[i]     Details\n");
			menu->printf("[=]     Add subdoot\n");
			menu->printf("[-]     Remove subdoot\n");
			menu->printf("[e]     Edit subdoot\n");
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

// Save doots to file
void App::save(const std::string &path)
{
	std::ofstream ofs(path);
	for (auto &doot : doot_list)
		write_doot(ofs, doot);
}
