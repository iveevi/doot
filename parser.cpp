#include "global.hpp"

///////////////////
// Parsing doots //
///////////////////

// Skip whitespace
void skip_whitespace(const std::string &glob, size_t &i)
{
	while (i < glob.size() && isspace(glob[i]))
		i++;
}

// Skip rest of line
void skip_line(const std::string &glob, size_t &i)
{
	while (i < glob.size() && glob[i] != '\n')
		i++;
}

// Check if substring at index matches, and advance index if so
bool check_substr(const std::string &glob, const std::string substr, size_t &i)
{
	if (glob.substr(i, substr.size()) == substr) {
		i += substr.size();
		return true;
	}

	return false;
}

SubDoot parse_subdoot(const std::string &glob, size_t &i)
{
	// Skip whitespace
	skip_whitespace(glob, i);

	// Get string in quotes
	std::string task;
	if (glob[i] == '"') {
		i++;
		while (i < glob.size() && glob[i] != '"') {
			task += glob[i];
			i++;
		}
		i++;
	} else {
		return SubDoot();
	}

	// Skip whitespace
	skip_whitespace(glob, i);

	// Get deadline (UNIX timestamp)
	int deadline;
	if (glob[i] == ':') {
		i++;
		deadline = std::stoi(glob.substr(i));
		i += std::to_string(deadline).size();
	} else {
		return SubDoot();
	}

	// Skip whitespace
	skip_whitespace(glob, i);

	// Get done status
	bool done;
	if (glob[i] == ':') {
		i++;
		done = (glob[i] == 't');
		i++;

		// Skip the rest of the line
		skip_line(glob, i);
	} else {
		return SubDoot();
	}

	// Return
	return SubDoot {task, deadline, done, false};
}

Dootling parse_dootling(const std::string &glob, size_t &i)
{
	// Static substrings
	static const std::string header = "[Dootling]";

	// Skip whitespace
	skip_whitespace(glob, i);

	// Check for [Doot]
	if (!check_substr(glob, header, i))
		return Dootling();

	// Skip white space and get name as a line
	skip_whitespace(glob, i);

	// Get name
	std::string name;
	while (i < glob.size() && glob[i] != '\n')
		name += glob[i++];

	// Get all subdoots
	SubDoots sub_doots;
	while (true) {
		SubDoot subdoot = parse_subdoot(glob, i);
		if (subdoot.task.empty())
			break;

		sub_doots.push_back(subdoot);
	}

	// Return
	return Dootling {name, sub_doots, false};
}

Doot parse_doot(const std::string &glob, size_t &i)
{
	// Static substrings
	static const std::string header = "[Doot]";

	// Skip whitespace
	skip_whitespace(glob, i);

	// Check for [Doot]
	if (!check_substr(glob, header, i))
		return Doot();

	// Skip white space and get name as a line
	skip_whitespace(glob, i);

	// Get name
	std::string name;
	while (i < glob.size() && glob[i] != '\n')
		name += glob[i++];

	// Get all the dootlings
	Dootlings dootlings;
	while (true) {
		Dootling dootling = parse_dootling(glob, i);
		if (dootling.title.empty())
			break;

		dootlings.push_back(dootling);
	}

	// Return doot
	return Doot {name, dootlings, false};
}

Doots parse_doots(const std::string &glob)
{
	size_t i = 0;

	Doots doots;
	while (true) {
		Doot doot = parse_doot(glob, i);
		if (doot.title.empty())
			break;

		doots.push_back(doot);
	}

	return doots;
}
