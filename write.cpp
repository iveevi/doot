#include "global.hpp"

/////////////////////////
// Write todos to file //
/////////////////////////

void write_subdoot(std::ostream &fout, const SubDoot &subdoot)
{
	fout << "\"" << subdoot.task << "\"" << ":" << subdoot.deadline
		<< ":" << std::boolalpha << subdoot.done << "\n";
}

void write_dootling(std::ostream &fout, const Dootling &dootling)
{
	fout << "[Dootling]\n" << dootling.title << "\n";

	for (const auto &subdoot : dootling.subdoots)
		write_subdoot(fout, subdoot);
}

void write_doot(std::ostream &fout, const Doot &doot)
{
	fout << "[Doot]\n" << doot.title << "\n";

	for (const auto &dootling : doot.dootlings)
		write_dootling(fout, dootling);
}

