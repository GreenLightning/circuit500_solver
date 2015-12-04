#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <queue>
#include <vector>
#include <chrono>

#include <boost/icl/interval_set.hpp>

#include "rgb_image.hpp"
#include "logger.hpp"
#include "solver/tile.hpp"
#include "solver/board.hpp"
#include "solver/reference.hpp"
#include "solver/configuration.hpp"
#include "solver/solution_list.hpp"
#include "solver/solution_painter.hpp"

using namespace std;
using namespace std::chrono;

namespace icl = boost::icl;

Configuration *load_level(string filename, RGB_Image **reference_images, bool &error);
void free_level(Configuration *level);

vector<Configuration> find_solution_list(Configuration level, long long int &solutions_checked);

void solve_levels(icl::interval_set<int> &level_set) {
	bool error = false;
	RGB_Image **reference_images = load_reference_images(error);
	if (!error) {
		Logger logger;
		for(icl::interval_set<int>::element_iterator level_it = elements_begin(level_set); level_it != elements_end(level_set); ++level_it) {
			int level_number = *level_it;
			cout << level_number << ": " << flush;
			bool level_error = false;
			ostringstream filename;
			filename << "data/levels/level_" << setfill('0') << setw(3) << level_number << ".png";
			Configuration *level = load_level(filename.str(), reference_images, level_error);
			if (!level_error) {
				long long int & solutions_checked = logger.start_search(level_number);
				vector<Configuration> list = find_solution_list(*level, solutions_checked);
				logger.stop_search();
				if (!list.empty()) {
					high_resolution_clock::time_point t0 = high_resolution_clock::now();
					bool error = false;
					RGB_Image image = Solution_Painter(*level, list, reference_images).paint(error);
					ostringstream filename;
					filename << "data/solutions/solution_" << setfill('0') << setw(3) << level_number << ".png";
					rgb_image_save(filename.str(), image, error);
					cout << (error ? rgb_image_error_text() : "success") << ": " << flush;
					high_resolution_clock::time_point t1 = high_resolution_clock::now();
					auto duration_ms = duration_cast<milliseconds>(t1 - t0).count();
					cout << "saving image took " << duration_ms << "ms: " << flush;
				}
			}
			free_level(level);
			cout << "done" << endl;
		}
	}
	free_reference_images(reference_images);
}

Configuration *load_level(string filename, RGB_Image **reference_images, bool &error) {
	if (error) return nullptr;
	Configuration *config = new Configuration{0};
	if (config == nullptr) {
		cout << "Error: out of memory while allocating configuration." << endl;
		error = true;
		return nullptr;
	}
	RGB_Image img = rgb_image_load(filename, level_pixel_width, level_pixel_height, error);
	for (int y = 0; !error && y < level_tile_height; ++y) {
		for (int x = 0; !error && x < level_tile_width; ++x) {
			RGB_Image level_tile = rgb_image_create_view(img, x * tile_size, y * tile_size, tile_size, tile_size, error);
			int reference_index = 0;
			for (; reference_index < number_of_references; ++reference_index)
				if (rgb_image_compare(level_tile, *reference_images[reference_index], error))
					break;
			if (reference_index < number_of_references) {
				config->board[board_position(x, y)] = reference_tiles[reference_index];
			} else {
				cout << "Unknown tile found." << endl;
				error = true;
			}
		}
	}
	if (error) {
		cout << "Error: " << rgb_image_error_text() << endl;
	}
	return config;
}

void free_level(Configuration *level) {
	delete level;
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

vector<Configuration> find_solution_list(Configuration level, long long int &solutions_checked) {
	Solution_List list;
	Board_State state;
	find_solution_helper(list, state, solutions_checked, level);
	return list.get();
}