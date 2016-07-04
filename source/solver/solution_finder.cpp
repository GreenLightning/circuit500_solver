#include "board.hpp"
#include "configuration.hpp"
#include "solution_list.hpp"

#include "solution_finder.hpp"

Solution_Finder::Solution_Finder(int max_taps) :
	max_taps(max_taps), solutions_checked(0) {}

void Solution_Finder::find(const Configuration& configuration, Board_Position start) {
	++solutions_checked;
	state.update(configuration.board);
	if (state.is_solved()) {
		list.append(configuration);
	} else if (configuration.tap_count < max_taps) {
		if (configuration.board.has_gap()) {
			Board_Position gap = configuration.board.get_gap();
			if (gap.has_up() && gap.get_up() < start) {
				Tap_Result result = with_tap(configuration, gap.get_up());
				find(result.config, Board_Position());
			}
			if (gap.has_down() && gap.get_down() < start) {
				Tap_Result result = with_tap(configuration, gap.get_down());
				find(result.config, Board_Position());
			}
			if (gap.has_left() && gap.get_left() < start) {
				Tap_Result result = with_tap(configuration, gap.get_left());
				find(result.config, Board_Position());
			}
			if (gap.has_right() && gap.get_right() < start) {
				Tap_Result result = with_tap(configuration, gap.get_right());
				find(result.config, Board_Position());
			}
		}
		for (Board_Position position = start; position < board_size; ++position) {
			Tap_Result result = with_tap(configuration, position);
			if (result.changed) {
				Board_Position new_start = position;
				if (result.moved) new_start = Board_Position();
				find(result.config, new_start);
			}
		}
	}
}

long long int Solution_Finder::get_solutions_checked() {
	return solutions_checked;
}

Solution_List Solution_Finder::get_solution_list() {
	return list;
}
