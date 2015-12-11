#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/icl/interval_set.hpp>

#include "rgb_image.hpp"
#include "logger.hpp"
#include "solver/tile.hpp"
#include "solver/board.hpp"
#include "solver/reference.hpp"
#include "solver/configuration.hpp"
#include "solver/solution_list.hpp"
#include "solver/solution_painter.hpp"

namespace fs = boost::filesystem;
namespace icl = boost::icl;

Configuration *load_level(std::string filename, RGB_Image **reference_images, bool &error);
void free_level(Configuration *level);

void solve_levels(icl::interval_set<int> &level_set, bool unsolved, Logger &logger, RGB_Image **reference_images);
void solve_level(int level_number, bool unsolved, Logger &logger, RGB_Image **reference_images);

std::string level_filename(int level_number);
std::string solution_filename(int level_number);

Solution_List find_solution_list(Configuration level, long long int &solutions_checked);
void find_solution_helper(Solution_List &list, Board_State &state, long long int &solutions_checked, const Configuration &solution);

Configuration *load_level(std::string filename, RGB_Image **reference_images, bool &error) {
	if (error) return nullptr;
	Configuration *config = new Configuration{0};
	if (config == nullptr) {
		std::cout << "Error: out of memory while allocating configuration." << std::endl;
		error = true;
		return nullptr;
	}
	RGB_Image img = RGB_Image::load(filename, level_pixel_width, level_pixel_height, error);
	for (int y = 0; !error && y < level_tile_height; ++y) {
		for (int x = 0; !error && x < level_tile_width; ++x) {
			RGB_Image level_tile = img.create_view(x * tile_size, y * tile_size, tile_size, tile_size, error);
			int reference_index = 0;
			for (; reference_index < number_of_references; ++reference_index)
				if (level_tile.equals(*reference_images[reference_index], error))
					break;
			if (reference_index < number_of_references) {
				config->board[board_position(x, y)] = reference_tiles[reference_index];
			} else {
				std::cout << "Unknown tile found." << std::endl;
				error = true;
			}
		}
	}
	if (error) {
		std::cout << "Error: " << RGB_Image::get_error_text() << std::endl;
	}
	return config;
}

void free_level(Configuration *level) {
	delete level;
}

void solve_levels(icl::interval_set<int> &level_set, bool unsolved, Logger &logger) {
	bool error = false;
	RGB_Image **reference_images = load_reference_images(error);
	if (!error)
		solve_levels(level_set, unsolved, logger, reference_images);
	free_reference_images(reference_images);
}

void solve_levels(icl::interval_set<int> &level_set, bool unsolved, Logger &logger, RGB_Image **reference_images) {
	for(icl::interval_set<int>::element_iterator level_it = elements_begin(level_set); level_it != elements_end(level_set); ++level_it)
		solve_level(*level_it, unsolved, logger, reference_images);
}

void solve_level(int level_number, bool unsolved, Logger &logger, RGB_Image **reference_images) {
	std::cout << level_number << ": " << std::flush;
	std::string solution_name = solution_filename(level_number);
	if (unsolved && fs::is_regular_file(fs::path(solution_name))) {
		std::cout << "skipping" << std::endl;
		return;
	}
	bool level_error = false;
	Configuration *level = load_level(level_filename(level_number), reference_images, level_error);
	if (!level_error) {
		long long int & solutions_checked = logger.start_search(level_number);
		Solution_List list = find_solution_list(*level, solutions_checked);
		logger.stop_search(list.get_tap_count(), list.get_action_count());
		if (!list.empty()) {
			bool error = false;
			RGB_Image image = Solution_Painter(*level, list.get_solutions(), reference_images).paint(error);
			image.save(solution_name, error);
			std::cout << (error ? RGB_Image::get_error_text() : "success") << ": " << std::flush;
		}
	}
	free_level(level);
	std::cout << "done" << std::endl;
}

std::string level_filename(int level_number) {
	std::ostringstream filename;
	filename << "data/levels/level_" << std::setfill('0') << std::setw(3) << level_number << ".png";
	return filename.str();
}

std::string solution_filename(int level_number) {
	std::ostringstream filename;
	filename << "data/solutions/solution_" << std::setfill('0') << std::setw(3) << level_number << ".png";
	return filename.str();
}

Solution_List find_solution_list(Configuration level, long long int &solutions_checked) {
	Solution_List list;
	Board_State state;
	find_solution_helper(list, state, solutions_checked, level);
	return list;
}

void find_solution_helper(Solution_List &list, Board_State &state, long long int &solutions_checked, const Configuration &solution) {
	++solutions_checked;
	update_board_state(solution.board, state);
	if (state.solved) {
		list.append(solution);
	} else if (solution.tap_count < maximum_number_of_taps) {
		for (Board_Position position = 0; position < board_size; ++position) {
			bool failed = false;
			Configuration next = with_tap(solution, position, failed);
			if (!failed) find_solution_helper(list, state, solutions_checked, next);
		}
	}
}