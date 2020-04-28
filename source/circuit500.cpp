#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "level_set.hpp"
#include "level_set_parser.hpp"
#include "logger.hpp"
#include "preparer.hpp"
#include "solver.hpp"

namespace fs = std::filesystem;

struct Options {
	bool help = false;
	bool log  = false;

	bool dry      = false;
	bool unsolved = false;

	bool prepare_all = false;
	bool solve_all   = false;
	bool handle_all  = false;

	int taps = 3;

	std::vector<std::string> prepare;
	std::vector<std::string> solve;
	std::vector<std::string> handle;
};

const char* help_message = R"DONE(Solves circuit500 levels.

All operations are performed in the 'data/' directory relative to the current
working directory, which should already exist and contain some reference images
for the solver inside 'data/reference/'.

Screenshots of the levels should be placed inside the 'data/raw/' directory.

During the preparation phase the program extracts the level number from the
screenshots and places a normalized version of the level area in an
appropriately named file inside the 'data/levels/' directory.

During the solving phase the program attempts to solve the levels and
puts solutions in the 'data/solutions/' directory.

Options:
  --help                 Prints this help message.
                         
  -l [ --log ]           Logs statistics about the solving phase into a log 
                         file inside 'data/logs/'.
                         
  -d [ --dry ]           Does not save solutions during the solving phase.
                         
  -u [ --unsolved ]      Solves only those levels for which it cannot already 
                         find a solution inside 'data/solutions/'.
                         
  -t [ --taps ] arg (=%d) Specifies the maximum number of taps to check for 
                         solutions.
                         Must be between %d and %d (both inclusive).
                         
  -p [ --prepare ] arg   Prepares the specified files.
                         Must be followed by one or more filenames.
                         Filenames must not include 'data/raw/'.
                         
  -P [ --prepare-all ]   Prepares all files in 'data/raw/'.
                         
  -s [ --solve ] arg     Solves the specified levels.
                         Must be followed by one or more level descriptions in 
                         the form of:
                          X:   where X is a level number between 1 and 500. 
                               Solves the specified level.
                          X-Y: where X and Y are level numbers between 1 and 
                               500 and X is less than or equal to Y. Solves all
                               levels between X and Y (both inclusive).
                         
  -S [ --solve-all ]     Solves all levels.
                         
  -h [ --handle ] arg    Prepares the specified files and solves the levels 
                         they contain.
                         
  -H [ --handle-all ]    Handles all files in 'data/raw/'.

)DONE";

void print_help_message() {
	Options default_options;
	printf(help_message, default_options.taps, Solver::tap_minimum(), Solver::tap_maximum());
}

Options parse_command_line_options(int argument_count, char** argument_values) {
	Options options;

	auto has_prefix = [](std::string&& s, std::string&& p) -> bool {
		return s.compare(0, p.size(), p) == 0;
	};

	for (int index = 1; index < argument_count; index++) {
		std::string argument = argument_values[index];

		auto get_value = [&]() -> std::string {
			index++;

			if (index >= argument_count)
				throw std::runtime_error("missing argument value for " + argument);

			return argument_values[index];
		};

		auto parse_multivalue = [&](std::vector<std::string>& values) {
			values.push_back(get_value());
			while (index + 1 < argument_count && !has_prefix(argument_values[index+1], "-")) {
				values.push_back(argument_values[index+1]);
				index++;
			}
		};

		if (argument == "--help") {
			options.help = true;

		} else if (argument == "--log" || argument == "-l") {
			options.log = true;

		} else if (argument == "--dry" || argument == "-d") {
			options.dry = true;

		} else if (argument == "--unsolved" || argument == "-u") {
			options.unsolved = true;

		} else if (argument == "--prepare-all" || argument == "-P") {
			options.prepare_all = true;

		} else if (argument == "--solve-all" || argument == "-S") {
			options.solve_all = true;

		} else if (argument == "--handle-all" || argument == "-H") {
			options.handle_all = true;

		} else if (argument == "-taps" || argument == "-t") {
			std::string value = get_value();
			std::regex regex("\\d{1,3}");
			std::smatch match;

			if (!regex_match(value, match, regex))
				throw std::runtime_error("expected tap count, but found: '" + value + "'");

			int parsed = std::stoi(match[0]);
			if (parsed < Solver::tap_minimum())
				throw std::runtime_error("tap count must be " + std::to_string(Solver::tap_minimum()) + " or more, but was: '" + value + "'");
			if (parsed > Solver::tap_maximum())
				throw std::runtime_error("tap count must be " + std::to_string(Solver::tap_maximum()) + " or less, but was: '" + value + "'");

			options.taps = parsed;

		} else if (argument == "--prepare" || argument == "-p") {
			parse_multivalue(options.prepare);

		} else if (argument == "--solve" || argument == "-s") {
			parse_multivalue(options.solve);

		} else if (argument == "--handle" || argument == "-h") {
			parse_multivalue(options.handle);

		} else {
			throw std::runtime_error("unknown argument " + argument);
		}
	}

	return options;
}

int main(int argument_count, char** argument_values) {
	try {
		Options options = parse_command_line_options(argument_count, argument_values);

		if (options.handle_all && !options.handle.empty())
			throw std::runtime_error("handle is not allowed if handle-all is used");

		if (options.solve_all && !options.solve.empty())
			throw std::runtime_error("solve is not allowed if solve-all is used");

		if (options.prepare_all && !options.prepare.empty())
			throw std::runtime_error("prepare is not allowed if prepare-all is used");

		if (options.help) {
			print_help_message();
		}

		fs::path reference_dir("data/reference/");

		if (!fs::is_directory(reference_dir)) {
			std::string error("could not find directory '" + reference_dir.string() + "'");
			if (!options.help)
				error.append("; use '--help' to get more information");
			throw std::runtime_error(error);
		}

		if (argument_count == 1) { // No options
			std::cout << "use '--help' to get usage information" << std::endl;
		}

		auto create_directory_if_not_exists = [](fs::path path) -> bool {
			if (fs::is_directory(path)) return true;
			return fs::create_directory(path);
		};

		create_directory_if_not_exists("data/levels/");
		create_directory_if_not_exists("data/raw/");
		create_directory_if_not_exists("data/solutions/");
		create_directory_if_not_exists("data/logs/");

		std::vector<fs::path> files_to_prepare;
		std::vector<fs::path> files_to_handle;
		Level_Set levels_to_solve;

		if (options.prepare_all) {
			for (auto&& entry : fs::directory_iterator("data/raw/")) {
				files_to_prepare.push_back(entry.path());
			}
		}
		if (!options.prepare.empty()) {
			fs::path dir = "data/raw";
			for (auto&& entry : options.prepare) {
				files_to_prepare.push_back(dir / entry);
			}
		}

		if (options.handle_all) {
			for (auto&& entry : fs::directory_iterator("data/raw/")) {
				files_to_handle.push_back(entry.path());
			}
		}
		if (!options.handle.empty()) {
			fs::path dir = "data/raw";
			for (auto&& entry : options.handle) {
				files_to_handle.push_back(dir / entry);
			}	
		}
		
		if (options.solve_all) {
			levels_to_solve.set_all();
		}
		if (!options.solve.empty()) {
			Level_Set_Parser(options.solve, levels_to_solve).parse();
		}

		std::string header;
		// create header for log
		// contains normalized list of arguments
		// normalized means not all arguments are mentioned,
		// the long form is always used and the order is fixed
		{
			if (options.log)
				header += " --log";

			if (levels_to_solve.is_full()) {
				header += " --solve-all";
			} else if (!levels_to_solve.is_empty()) {
				header += " --solve";
				size_t low = level_begin, high;
				while (true) {
					while (low != level_end && !levels_to_solve.is_set(low)) ++low;
					high = low;
					while (high != level_end && levels_to_solve.is_set(high)) ++high;
					if (low == level_end) break;
					header += " " + std::to_string(low);
					if (high - low > 1) header += "-" + std::to_string(high - 1);
					low = high;
				}
			}

			if (options.unsolved)
				header += " --unsolved";

			header += " --taps " + std::to_string(options.taps);

			if (!header.empty()) header.erase(0, 1);
		}

		Logger* logger = options.log ? Logger::create_file_logger(header) : Logger::create_fake_logger(header);
		if (!logger)
			throw std::runtime_error("out of memory: failed to create logger");

		{
			Preparer preparer;
			if (preparer.is_initialized()) {
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
					if (level_number != 0) levels_to_solve.set(level_number);
				}
			}
		}

		{
			Solver solver(*logger, options.taps);
			solver.unsolved = options.unsolved;
			solver.dry = options.dry;
			for (size_t i = level_begin; i != level_end; ++i)
				if (levels_to_solve.is_set(i))
					solver.solve_level(i);
		}

		delete logger;

		return EXIT_SUCCESS;
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
