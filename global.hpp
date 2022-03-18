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

///////////////
// Functions //
///////////////

std::string read_glob(const std::string &);
Doots parse_doots(const std::string &);

#endif
