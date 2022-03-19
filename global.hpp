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
	enum WinContent : int {
		WIN_CONTENT_DOOTS = 0,
		WIN_CONTENT_DOOTLINGS,
		WIN_CONTENT_SUBDOOTS
	};

	// Current window
	WinContent win_content = WIN_CONTENT_DOOTS;

	// All windows
	tuicpp::BoxedWindow whole;

	tuicpp::DecoratedWindow doots;
	tuicpp::DecoratedWindow dootlings;
	tuicpp::DecoratedWindow subdoots;
	tuicpp::Table <SubDoot> subdoot_table;

	tuicpp::BoxedWindow editor;
public:
	// Constructor initializes all the windows
	App();

	// Run application
	void run();
};

///////////////
// Functions //
///////////////

std::string read_glob(const std::string &);
std::string time_to_string(int);

Doots parse_doots(const std::string &);

void write_doot(std::ostream &, const Doot &);

#endif
