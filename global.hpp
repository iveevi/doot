#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "tuicpp/tuicpp.hpp"

/////////////////////////////////
// Doot and related structures //
/////////////////////////////////

// Subtasks
struct SubDoot {
	std::string	task;
	int		deadline;
	bool		done;

	// TODO: properties like
	// - priority
	// - color yellow if close to deadline
	// - color red if deadline is passed
	// - time before warning notification
	// - time estimate
	//
	// TODO: at some point add some machine
	// learning to schedule tasks
};

using SubDoots = std::vector <SubDoot>;

// A single task
struct Dootling {
	std::string	title;
	SubDoots	subdoots;
	bool		done;
};

using Dootlings = std::vector <Dootling>;

// A group of related tasks (e.g. "work" and "personal")
struct Doot {
	std::string	title;
	Dootlings	dootlings;
	bool		done;
};

using Doots = std::vector <Doot>;

///////////////////////
// Application class //
///////////////////////

class App {
	// Enumerations for current window
	static constexpr int WIN_CONTENT_DOOTS = 0;
	static constexpr int WIN_CONTENT_DOOTLINGS = 1;
	static constexpr int WIN_CONTENT_SUBDOOTS = 2;

	// Current window
	int win_content = WIN_CONTENT_DOOTS;

	// All windows
	tuicpp::BoxedWindow *whole;

	tuicpp::DecoratedWindow *menu;
	tuicpp::DecoratedWindow *doots;
	tuicpp::DecoratedWindow *dootlings;
	tuicpp::DecoratedWindow *subdoots;
	tuicpp::DecoratedWindow *editor = nullptr;

	tuicpp::Table <SubDoot> *subdoot_table;

	// Doots
	Doots doot_list;

	// Run variables
	int curr_doot = 0;
	int curr_dootling = 0;
	int curr_subdoot = 0;

	// Check inputs
	void check_up();
	void check_down();
	void check_inputs(int);

	// Creaing windows
	void create_doot_editor();
	void create_dootling_editor();
	void create_subdoot_editor();
public:
	// Constructor initializes all the windows
	App(const Doots &);
	
	// Destructor
	~App();

	// Run application
	void run();

	// Static variables and functions
	static const tuicpp::Table <SubDoot> ::Headers headers;
	static const tuicpp::Table <SubDoot> ::From from;

	static std::string subdoot_element(const SubDoot &, size_t);
};

///////////////
// Functions //
///////////////

std::string read_glob(const std::string &);
std::string time_to_string(int);

Doots parse_doots(const std::string &);

void write_doot(std::ostream &, const Doot &);

#endif
