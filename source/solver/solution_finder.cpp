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
	} else if (configuration.tap_count < max_taps && (list.empty() || configuration.action_count <= list.get_action_count())) {
		if (configuration.board.has_gap()) {
			Board_Position gap = configuration.board.get_gap();
			if (gap.has_up() && gap.get_up() < start) {
				Board_Position pos = gap.get_up();
				Tap_Result result = with_tap(configuration, pos);
				find(result.config, pos.has_up() ? pos.get_up() : Board_Position());
			}
			if (gap.has_down() && gap.get_down() < start) {
				Tap_Result result = with_tap(configuration, gap.get_down());
				find(result.config, gap.has_up() ? gap.get_up() : Board_Position());
			}
			if (gap.has_left() && gap.get_left() < start) {
				Board_Position pos = gap.get_left();
				Tap_Result result = with_tap(configuration, pos);
				find(result.config, pos.has_up() ? pos.get_up() : Board_Position());
			}
			if (gap.has_right() && gap.get_right() < start) {
				Tap_Result result = with_tap(configuration, gap.get_right());
				find(result.config, gap.has_up() ? gap.get_up() : Board_Position());
			}
		}
		for (Board_Position position = start; position < board_size; ++position) {
			Tap_Result result = with_tap(configuration, position);
			if (result.changed) {
				Board_Position new_start = position;
				if (result.moved) new_start = result.top.has_up() ? result.top.get_up() : Board_Position();
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
