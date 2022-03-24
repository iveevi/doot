#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

#include "nabu/nabu.hpp"
#include "tuicpp/tuicpp.hpp"

/////////////////////////////////
// Doot and related structures //
/////////////////////////////////

// Subtasks
// TODO: calendar view of all tasks (7 days per row, etc. -- weekly and monthly
// as well as a view of all tasks due today)
struct SubDoot {
	std::string	task;
	long int	deadline;
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

	// Deleting
	void delete_doot();
	void delete_dootling();
	void delete_subdoot();

	// Editing
	void edit_doot();
	void edit_dootling();
	void edit_subdoot();
public:
	// Constructor initializes all the windows
	App(const Doots &);
	
	// Destructor
	~App();

	// Run application
	void run();

	// Save to file
	void save(const std::string &);

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

////////////////////
// Parsers (nabu) //
////////////////////

struct date_time {
	int hour;
	int minute;
	int second;
};

struct date {
	int month;
	int day;
	int year;
};

using nabu::ret;
using nabu::Tret;
using nabu::get;
using nabu::rules::rule;
using nabu::rules::lit;
using nabu::rules::multiplex;
using nabu::rules::sequential;
using nabu::StringFeeder;
using nabu::Feeder;

/* TODO: allow 12-hour time format
constexpr char am_lower[] = "am";
constexpr char am_upper[] = "AM";
constexpr char pm_lower[] = "pm";
constexpr char pm_upper[] = "PM";
*/

template <>
inline std::string nabu::convert_string <date_time> (const date_time &pr)
{
	return std::to_string(pr.hour) + ":"
		+ std::to_string(pr.minute) + ":"
		+ std::to_string(pr.second);
}

template <>
inline std::string nabu::convert_string <date> (const date &pr)
{
	return std::to_string(pr.month) + "/"
		+ std::to_string(pr.day) + "/"
		+ std::to_string(pr.year);
}

template <>
struct nabu::rules::rule <date_time> : public sequential <
		int, lit <':'>, int
	> {
	
	// Override value
	static ret value(Feeder *fd) {
		ret rptr = _value(fd);
		if (!rptr)
			return rptr;

		auto rv = getrv(rptr);
		int hour = get <int> (rv[0]);
		int minute = get <int> (rv[2]);

		return ret(new Tret <date_time> ({hour, minute, 0}));
	}
};

template <>
struct nabu::rules::rule <date> : public sequential <
		int, lit <'/'>, int
	> {
	
	// Override value
	static ret value(Feeder *fd) {
		ret rptr = _value(fd);
		if (!rptr)
			return rptr;

		auto rv = getrv(rptr);
		int month = get <int> (rv[0]);
		int day = get <int> (rv[2]);
		// int year = get <int> (rv[4]);

		return ret(new Tret <date> ({month, day, 2022}));
	}
};

#endif
