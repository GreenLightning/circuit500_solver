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

#include <boost/date_time.hpp>
#include <boost/icl/interval_set.hpp>

#include "rgb_image.hpp"
#include "solver/tile.hpp"
#include "solver/board.hpp"
#include "solver/reference.hpp"

using namespace std;
using namespace std::chrono;

namespace pt = boost::posix_time;
namespace icl = boost::icl;

const int maximum_number_of_taps = 6;

struct Configuration {
	Board board;
	Board_Position taps[maximum_number_of_taps];
	uint8_t tap_count;
	uint8_t action_count;
};

Configuration with_move(Configuration config, Board_Position tap_position, Board_Position gap_position) {
	Tile tap_tile = config.board[tap_position];
	config.board[tap_position] = config.board[gap_position];
	config.board[gap_position] = tap_tile;
	if (can_rotate(tap_tile)) ++config.action_count;
	return config;
}

Configuration with_tap(Configuration config, Board_Position tap, bool &failed) {
	config.taps[config.tap_count++] = tap;
	if (has_up(tap)) {
		Board_Position up = get_up(tap);
		if (is_gap(config.board[up])) return with_move(config, tap, up);
	}
	if (has_right(tap)) {
		Board_Position right = get_right(tap);
		if (is_gap(config.board[right])) return with_move(config, tap, right);
	}
	if (has_down(tap)) {
		Board_Position down = get_down(tap);
		if (is_gap(config.board[down])) return with_move(config, tap, down);
	}
	if (has_left(tap)) {
		Board_Position left = get_left(tap);
		if (is_gap(config.board[left])) return with_move(config, tap, left);
	}
	if (can_rotate(config.board[tap]) && get_rotations(config.board[tap]) < 3) {
		config.board[tap] = get_rotated(config.board[tap]);
		++config.action_count;
		return config;
	}
	failed = true;
	return config;
}

Configuration with_tap(Configuration config, Board_Position tap) {
	bool failed = false;
	return with_tap(config, tap, failed);
}

Configuration *load_level(string filename, RGB_Image **reference_images, bool &error);
void free_level(Configuration *level);

vector<Configuration> find_solution_list(Configuration level, long long int &solutions_checked);

int solution_list_pixel_width(vector<Configuration> &list);
int solution_list_pixel_height(vector<Configuration> &list);
void draw_solution_list(RGB_Image &destination, Configuration level, vector<Configuration> &list, RGB_Image **reference_images);
int solution_pixel_width(Configuration solution);
void draw_solution(RGB_Image &destination, Configuration level, Configuration solution, RGB_Image **reference_images);
void draw_board(RGB_Image &destination, Board &board, Board_State &state, RGB_Image **reference_images);

void solve_levels(icl::interval_set<int> &level_set) {
	bool error = false;
	RGB_Image **reference_images = load_reference_images(error);
	if (!error) {
		ostringstream logname;
		pt::time_facet *facet = new pt::time_facet();
		facet->format("%Y-%m-%d_%H.%M.%S");
		logname.imbue(std::locale(logname.getloc(), facet));
		logname << "data/logs/log_" << pt::microsec_clock::local_time() << ".txt";
		std::cout << logname.str() << std::endl;
		ofstream log(logname.str());
		log << "level; solutions; nanoseconds\n";
		for(icl::interval_set<int>::element_iterator level_it = elements_begin(level_set); level_it != elements_end(level_set); ++level_it) {
			int level_number = *level_it;
			cout << level_number << ": " << flush;
			log << setw(5) << level_number << ";";
			bool level_error = false;
			ostringstream filename;
			filename << "data/levels/level_" << setfill('0') << setw(3) << level_number << ".png";
			Configuration *level = load_level(filename.str(), reference_images, level_error);
			if (!level_error) {
				long long int solutions_checked = 0;
				high_resolution_clock::time_point t0 = high_resolution_clock::now();
				vector<Configuration> list = find_solution_list(*level, solutions_checked);
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				auto duration_ms = duration_cast<milliseconds>(t1 - t0).count();
				cout << "checked " << solutions_checked << " solutions: ";
				cout << "finding solutions took " << duration_ms << "ms: " << flush;
				auto duration_ns = duration_cast<nanoseconds>(t1 - t0).count();
				log << setw(10) << solutions_checked << ";" << setw(12) << duration_ns;
				if (!list.empty()) {
					t0 = high_resolution_clock::now();
					bool error = false;
					int solution_width = solution_list_pixel_width(list);
					int solution_height = solution_list_pixel_height(list);
					RGB_Image test = rgb_image_create(solution_width + 2 * tile_size, solution_height + 2 * tile_size, 0x000000, error);
					if (!error) {
						RGB_Image dst = rgb_image_create_view(test, tile_size, tile_size, solution_width, solution_height, error);
						draw_solution_list(dst, *level, list, reference_images);
						ostringstream filename;
						filename << "data/solutions/solution_" << setfill('0') << setw(3) << level_number << ".png";
						rgb_image_save(filename.str(), test, error);
						cout << (error ? rgb_image_error_text() : "success") << ": " << flush;
					} else {
						cout << rgb_image_error_text() << flush;
					}
					t1 = high_resolution_clock::now();
					duration_ms = duration_cast<milliseconds>(t1 - t0).count();
					cout << "saving image took " << duration_ms << "ms: " << flush;
				}
			}
			free_level(level);
			cout << "done" << endl;
			log << "\n";
			if (level_number % 10 == 0) log.flush();
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

class Solution_List {
public:
	Solution_List() : found(false) {}
	bool should_skip(const Configuration &config) { return found && config.action_count > action_count; }
	void append(const Configuration &config) {
		if (!found) {
			found = true;
			tap_count = config.tap_count;
			action_count = config.action_count;
			solutions.push_back(config);
		} else if (config.tap_count <= tap_count && config.action_count <= action_count) {
			if (config.tap_count < tap_count || config.action_count < action_count) {
				solutions.clear();
				tap_count = config.tap_count;
				action_count = config.action_count;
				solutions.push_back(config);
			} else {
				bool found_equal = false;
				for (auto &&solution : solutions) {
					if (boards_are_equal(config.board, solution.board)) {
						found_equal = true;
						break;
					}
				}
				if (!found_equal) solutions.push_back(config);
			}
		}
	}
	vector<Configuration> get() { return solutions; }
private:
	bool found;
	uint8_t tap_count;
	uint8_t action_count;
	vector<Configuration> solutions;
};

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



int solution_list_pixel_width(vector<Configuration> &list) {
	int width = 0;
	for(auto solution = list.begin(); solution != list.end(); ++solution)
		width = max(width, solution_pixel_width(*solution));
	return width;
}

int solution_list_pixel_height(vector<Configuration> &list) {
	return list.size() * (level_pixel_height + tile_size) - tile_size;
}

void draw_solution_list(RGB_Image &destination, Configuration level, vector<Configuration> &list, RGB_Image **reference_images) {
	bool error = false;
	int offset = 0;
	for (auto solution = list.begin(); solution != list.end(); ++solution, ++offset) {
		RGB_Image img = rgb_image_create_view(destination, 0, offset * (level_pixel_height + tile_size), destination.getWidth(), level_pixel_height, error);
		draw_solution(img, level, *solution, reference_images);
	}
}

int solution_pixel_width(Configuration solution) {
	return solution.tap_count * (level_pixel_width + tile_size) + level_pixel_width;
}

void draw_solution(RGB_Image &destination, Configuration level, Configuration solution, RGB_Image **reference_images) {
	Board_State state;
	bool error = false;
	int offset = 0;
	for (int step = 0; step < solution.tap_count; ++step, ++offset) {
		RGB_Image img = rgb_image_create_view(destination, offset * (level_pixel_width + tile_size), 0, level_pixel_width, destination.getHeight(), error);
		update_board_state(level.board, state);
		draw_board(img, level.board, state, reference_images);
		Board_Position tap = solution.taps[step];
		RGB_Image tap_img = rgb_image_create_view(img, get_x(tap) * tile_size, get_y(tap) * tile_size, tile_size, tile_size, error);
		rgb_image_tint(tap_img, 0x80ff0000, error);
		level = with_tap(level, tap);
	}
	RGB_Image img = rgb_image_create_view(destination, offset * (level_pixel_width + tile_size), 0, level_pixel_width, destination.getHeight(), error);
	update_board_state(level.board, state);
	draw_board(img, level.board, state, reference_images);
}

void draw_board(RGB_Image &destination, Board &board, Board_State &state, RGB_Image **reference_images) {
	bool error = false;
	for (int y = 0; y < level_tile_height; ++y) {
		for (int x = 0; x < level_tile_width; ++x) {
			Board_Position position = board_position(x, y);
			RGB_Image dst_tile = rgb_image_create_view(destination, x * tile_size, y * tile_size, tile_size, tile_size, error);
			RGB_Image &ref_img = *reference_images[get_reference_index(board[position], state.filled[position])];
			rgb_image_copy_from_to(ref_img, dst_tile, error);
		}
	}
}