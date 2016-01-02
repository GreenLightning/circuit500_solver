#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "logger.hpp"
#include "rgb_image.hpp"
#include "solver/board.hpp"
#include "solver/configuration.hpp"
#include "solver/reference.hpp"
#include "solver/solution_list.hpp"
#include "solver/solution_painter.hpp"
#include "solver/tile.hpp"

#include "solver.hpp"

namespace fs = boost::filesystem;

Solver::Solver(Logger &logger) : logger(logger), unsolved(false), dry(false) {
	bool error = false;
	reference_images = load_reference_images(error);
	if (error)
		throw std::runtime_error("could not load reference images");
}

Solver::~Solver() {
	free_reference_images(reference_images);
}

void Solver::solve_level(int level_number) {
	std::cout << level_number << ": " << std::flush;
	std::string solution_name = solution_filename(level_number);
	if (unsolved && fs::is_regular_file(fs::path(solution_name))) {
		std::cout << "skipping" << std::endl;
		return;
	}
	bool level_error = false;
	Configuration *level = load_level(level_filename(level_number), level_error);
	if (!level_error) {
		long long int & solutions_checked = logger.start_search(level_number);
		Solution_List list = find_solution_list(*level, solutions_checked);
		logger.stop_search(list.get_tap_count(), list.get_action_count());
		if (!list.empty()) {
			bool error = false;
			if (!dry) {
				RGB_Image image = Solution_Painter(*level, list.get_solutions(), reference_images).paint(error);
				image.save(solution_name, error);
			}
			std::cout << (error ? RGB_Image::get_error_text() : "success") << ": " << std::flush;
		} else {
			std::cout << "failure: " << std::flush;
		}
	}
	free_level(level);
	std::cout << "done" << std::endl;
}

Configuration *Solver::load_level(std::string filename, bool &error) {
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
				config->board[Board_Position(x, y)] = reference_tiles[reference_index];
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

void Solver::free_level(Configuration *level) {
	delete level;
}

std::string Solver::level_filename(int level_number) {
	std::ostringstream filename;
	filename << "data/levels/level_" << std::setfill('0') << std::setw(3) << level_number << ".png";
	return filename.str();
}

std::string Solver::solution_filename(int level_number) {
	std::ostringstream filename;
	filename << "data/solutions/solution_" << std::setfill('0') << std::setw(3) << level_number << ".png";
	return filename.str();
}

Solution_List Solver::find_solution_list(Configuration level, long long int &solutions_checked) {
	Solution_List list;
	Board_State state;
	find_solution_helper(list, state, solutions_checked, level);
	return list;
}

void Solver::find_solution_helper(Solution_List &list, Board_State &state, long long int &solutions_checked, const Configuration &solution) {
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