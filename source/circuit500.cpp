#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

#include "level_set_parser.hpp"
#include "logger.hpp"
#include "preparer.hpp"
#include "solver.hpp"

namespace fs = boost::filesystem;
namespace icl = boost::icl;
namespace opt = boost::program_options;

void check_conflicting_options(opt::variables_map &variables, std::string master, std::string slave);
bool create_directory_if_not_exists(fs::path path);
void prepare_and_handle_files(
	std::vector<fs::path> &files_to_prepare,
	std::vector<fs::path> &files_to_handle,
	icl::interval_set<int> &levels_to_solve);
void solve_levels(
	icl::interval_set<int> &levels_to_solve,
	opt::variables_map &variables,
	Logger &logger);

int main(int argument_count, char **argument_values) {
	try {
		opt::options_description description(
			"Solves circuit500 levels.\n\n"

			"All operations are performed in the 'data/' directory relative to the current\n"
			"working directory, which should already exist and contain some reference images\n"
			"for the solver inside 'data/reference/'.\n\n"

			"Screenshots of the levels should be placed inside the 'data/raw/' directory.\n\n"

			"During the preparation phase the program extracts the level number from the\n"
			"screenshots and places a normalized version of the level area in an\n"
			"appropriately named file inside the 'data/levels/' directory.\n\n"

			"During the solving phase the program attempts to solve the levels and\n"
			"puts solutions in the 'data/solutions/' directory.\n\n"

			"Options");

		description.add_options()
			("help",
				"Prints this help message.\n")

			("log,l",
				"Logs statistics about the solving phase into a log file inside 'data/logs/'.\n")

			("dry,d",
				"Does not save solutions during the solving phase.\n")

			("unsolved,u",
				"Solves only those levels for which it cannot already find a solution inside "
				"'data/solutions/'.\n")

			("prepare,p", opt::value<std::vector<std::string>>()->multitoken(),
				"Prepares the specified files.\n"
				"Must be followed by one or more filenames.\n"
				"Filenames must not include 'data/raw/'.\n")

			("prepare-all,P",
				"Prepares all files in 'data/raw/'.\n")

			("solve,s", opt::value<std::vector<std::string>>()->multitoken(),
				"Solves the specified levels.\n"
				"Must be followed by one or more level descriptions in the form of:\n"
				" X:   \twhere X is a level number between 1 and 500. "
				"Solves the specified level.\n"
				" X-Y: \twhere X and Y are level numbers between 1 and 500 and X is less than or "
				"equal to Y. Solves all levels between X and Y (both inclusive).\n")

			("solve-all,S",
				"Solves all levels.\n")

			("handle,h", opt::value<std::vector<std::string>>()->multitoken(),
				"Prepares the specified files and solves the levels they contain.\n")

			("handle-all,H",
				"Handles all files in 'data/raw/'.\n")
		;

		opt::options_description helper_description;
		helper_description.add_options()
			("__unrecognized__", opt::value<std::vector<std::string>>())
		;

		opt::options_description cmdline_options;
		cmdline_options.add(description).add(helper_description);

		opt::positional_options_description positional_description;
		positional_description.add("__unrecognized__", -1);

		opt::variables_map variables;
		opt::store(opt::command_line_parser(argument_count, argument_values)
			.options(cmdline_options).positional(positional_description).run(), variables);
		opt::notify(variables);

		if (variables.count("__unrecognized__"))
			throw std::runtime_error("unexpected argument: '" + variables["__unrecognized__"].as<std::vector<std::string>>()[0] + "'");
		check_conflicting_options(variables, "handle-all", "handle");
		check_conflicting_options(variables, "solve-all", "solve");
		check_conflicting_options(variables, "prepare-all", "prepare");

		if (variables.count("help")) {
			std::cout << description << std::endl;
		}

		fs::path reference_dir("data/reference/");

		if (!fs::is_directory(reference_dir)) {
			std::string error("could not find directory '" + reference_dir.string() + "'");
			if (!variables.count("help"))
				error.append("; use '--help' to get more information");
			throw std::runtime_error(error);
		}

		if (argument_count == 1) { // No options
			std::cout << "use '--help' to get usage information" << std::endl;
		}

		create_directory_if_not_exists("data/levels/");
		create_directory_if_not_exists("data/raw/");
		create_directory_if_not_exists("data/solutions/");
		create_directory_if_not_exists("data/logs/");

		std::vector<fs::path> files_to_prepare;
		std::vector<fs::path> files_to_handle;
		icl::interval_set<int> levels_to_solve;

		if (variables.count("prepare-all")) {
			for (auto &&entry : fs::directory_iterator("data/raw/")) {
				files_to_prepare.push_back(entry.path());
			}
		}
		if (variables.count("prepare")) {
			fs::path dir = "data/raw";
			std::vector<std::string> input_list = variables["prepare"].as<std::vector<std::string>>();
			for (auto &&entry : input_list) {
				files_to_prepare.push_back(dir / entry);
			}
		}

		if (variables.count("handle-all")) {
			for (auto &&entry : fs::directory_iterator("data/raw/")) {
				files_to_handle.push_back(entry.path());
			}
		}
		if (variables.count("handle")) {
			fs::path dir = "data/raw";
			std::vector<std::string> input_list = variables["handle"].as<std::vector<std::string>>();
			for (auto &&entry : input_list) {
				files_to_handle.push_back(dir / entry);
			}	
		}
		
		if (variables.count("solve-all")) {
			levels_to_solve.insert(icl::interval<int>::closed(1, 500));
		}
		if (variables.count("solve")) {
			std::vector<std::string> input_list = variables["solve"].as<std::vector<std::string>>();
			levels_to_solve += Level_Set_Parser(input_list).parse();
		}

		Logger *logger = variables.count("log") ? Logger::create_file_logger() : Logger::create_fake_logger();
		if (!logger) {
			throw std::runtime_error("out of memory: failed to create logger");
		}

		prepare_and_handle_files(files_to_prepare, files_to_handle, levels_to_solve);
		solve_levels(levels_to_solve, variables, *logger);

		delete logger;

		return EXIT_SUCCESS;
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

void check_conflicting_options(opt::variables_map &variables, std::string master, std::string slave) {
	if (variables.count(master) && variables.count(slave))
			throw std::runtime_error(slave + " is not allowed if " + master + " is used");
}

bool create_directory_if_not_exists(fs::path path) {
	if (fs::is_directory(path)) return true;
	return fs::create_directory(path);
}

void prepare_and_handle_files(
	std::vector<fs::path> &files_to_prepare,
	std::vector<fs::path> &files_to_handle,
	icl::interval_set<int> &levels_to_solve) {

	Preparer preparer;
	if (!preparer.is_initialized()) return;

	int count = 0, total = files_to_prepare.size() + files_to_handle.size();

	for (auto&& it : files_to_prepare) {
		int progress = 100 * (++count) / total;
		std::cout << std::setfill(' ') << std::setw(3) << progress << "%: " << it.generic_string() << ": ";
		preparer.prepare(it);
	}

	for (auto&& it : files_to_handle) {
		int progress = 100 * (++count) / total;
		std::cout << std::setfill(' ') << std::setw(3) << progress << "%: " << it.generic_string() << ": ";
		int level_number = preparer.prepare(it);
		if (level_number != 0) levels_to_solve.insert(level_number);
	}
}

void solve_levels(icl::interval_set<int> &level_set, opt::variables_map &variables, Logger &logger) {
	Solver solver(logger);
	solver.unsolved = variables.count("unsolved");
	solver.dry = variables.count("dry");
	for(icl::interval_set<int>::element_iterator level_it = elements_begin(level_set);
		level_it != elements_end(level_set);
		++level_it)
		solver.solve_level(*level_it);
}