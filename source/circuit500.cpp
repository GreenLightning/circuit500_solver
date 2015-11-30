#include <iostream>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <string>
#include <cstdlib>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/icl/interval_set.hpp>

#include "level_set_parser.hpp"
#include "preprocessor.hpp"
#include "solver.hpp"

namespace fs = boost::filesystem;
namespace icl = boost::icl;
namespace opt = boost::program_options;

int main(int argument_count, char **argument_values) {
	try {
		opt::options_description description(
			"Solves circuit500 levels.\n\n"

			"All operations are performed in the 'data/' directory relative to the current\n"
			"working directory, which should already exist and contain some reference images\n"
			"for the solver inside 'data/reference/'.\n\n"

			"Screenshots of the levels should be placed inside the 'data/raw/' directory.\n"
			"Please create the directories 'data/levels/' and 'data/solutions/' if they do\n"
			"not exist yet, or the solver will fail to save the images.\n\n"

			"During the preparation phase the program extracts the level number from the\n"
			"screenshots and places a normalized version of the level area in an\n"
			"appropriately named file inside the 'data/levels/' directory.\n\n"

			"During the solving phase the program attempts to solve the levels and\n"
			"puts solutions in the 'data/solutions/' directory.\n\n"

			"Options");
		description.add_options()
			("help",
				"Prints this help message.")
			("prepare,p", opt::value<std::vector<std::string>>()->multitoken(),
				"Prepares the specified files.\n"
				"Must be followed by one or more filenames.\n"
				"Filenames must not include 'data/raw/'.")
			("prepare-all,P",
				"Prepares all files in 'data/raw/'.")
			("solve,s", opt::value<std::vector<std::string>>()->multitoken(),
				"Solves the specified levels.\n"
				"Must be followed by one or more level descriptions in the form of:\n"
				" X:   \twhere X is a level number between 1 and 500."
					"Solves the specified level.\n"
				" X-Y: \twhere X and Y are level numbers between 1 and 500 and X is less than or equal to Y."
					"Solves all levels between X and Y (both inclusive).")
			("solve-all,S",
				"Solves all levels.")
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
		if (variables.count("prepare-all") && variables.count("prepare"))
			throw std::runtime_error("prepare is not allowed if prepare-all is used");
		if (variables.count("solve-all") && variables.count("solve"))
			throw std::runtime_error("solve is not allowed if solve-all is used");

		if (argument_count == 1 || variables.count("help")) {
			std::cout << description << std::endl;
			return EXIT_FAILURE;
		}

		std::vector<fs::path> files_to_prepare;
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
		
		if (variables.count("solve-all")) {
			levels_to_solve.insert(icl::interval<int>::closed(1, 500));
		}
		if (variables.count("solve")) {
			std::vector<std::string> input_list = variables["solve"].as<std::vector<std::string>>();
			levels_to_solve += Level_Set_Parser(input_list).parse();
		}

		prepare_files(files_to_prepare);
		solve_levels(levels_to_solve);

		return EXIT_SUCCESS;
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}