#include "global.hpp"

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
