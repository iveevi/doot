#include "global.hpp"

///////////////////////
// Utility functions //
///////////////////////

// Read file into string
std::string read_glob(const std::string &path)
{
	// Open file
	std::ifstream file(path);

	// Check if file is open
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
		return "";
	}

	// Read file
	std::string content(
		(std::istreambuf_iterator <char> (file)),
		(std::istreambuf_iterator <char> ())
	);

	// Close file
	file.close();

	return content;
}

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

