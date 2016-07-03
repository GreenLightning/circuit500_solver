#include "board.hpp"
#include "tile.hpp"

#include "configuration.hpp"

void with_move(Tap_Result& result, Board_Position tap_position) {
	if (can_rotate(result.config.board[tap_position])) ++result.config.action_count;
	result.config.board.swap_with_gap(tap_position);
}

Tap_Result with_tap(const Configuration& input, Board_Position tap) {
	Tap_Result result { input, /* changed = */ true };
	Configuration& config = result.config;
	config.taps[config.tap_count++] = tap;
	if (tap.has_up() && config.board.is_gap(tap.get_up())) {
		with_move(result, tap);
		return result;
	} else if (tap.has_right() && config.board.is_gap(tap.get_right())) {
		with_move(result, tap);
		return result;
	} else if (tap.has_down() && config.board.is_gap(tap.get_down())) {
		with_move(result, tap);
		return result;
	} else if (tap.has_left() && config.board.is_gap(tap.get_left())) {
		with_move(result, tap);
		return result;
	} else if (can_rotate(config.board[tap]) && get_rotations(config.board[tap]) < 3) {
		config.board.rotate(tap);
		++config.action_count;
		return result;
	} else {
		result.changed = false;
		return result;
	}
}
