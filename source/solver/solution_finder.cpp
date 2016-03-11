#include "board.hpp"
#include "configuration.hpp"
#include "solution_list.hpp"

#include "solution_finder.hpp"

Solution_Finder::Solution_Finder(int min_taps, int max_taps) :
	min_taps(min_taps), max_taps(max_taps), solutions_checked(0) {}

void Solution_Finder::find(const Configuration& configuration) {
	if (configuration.tap_count + 1 < min_taps) {
		for (Board_Position position = 0; position < board_size; ++position) {
			bool error = false;
			Configuration next = with_tap(configuration, position, error);
			if (!error) find(next);
		}
	} else {
		for (Board_Position position = 0; position < board_size; ++position) {
			bool error = false;
			Configuration next = with_tap(configuration, position, error);
			if (!error) find_all(next);
		}
	}
}

void Solution_Finder::find_all(const Configuration& configuration) {
	++solutions_checked;
	state.update(configuration.board);
	if (state.is_solved()) {
		list.append(configuration);
	} else if (configuration.tap_count < max_taps) {
		for (Board_Position position = 0; position < board_size; ++position) {
			bool error = false;
			Configuration next = with_tap(configuration, position, error);
			if (!error) find_all(next);
		}
	}
}

long long int Solution_Finder::get_solutions_checked() {
	return solutions_checked;
}

Solution_List Solution_Finder::get_solution_list() {
	return list;
}
