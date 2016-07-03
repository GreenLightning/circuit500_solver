#include "board.hpp"
#include "tile.hpp"

#include "configuration.hpp"

Configuration with_move(Configuration& config, Board_Position tap_position) {
	if (can_rotate(config.board[tap_position])) ++config.action_count;
	config.board.swap_with_gap(tap_position);
	return config;
}

Configuration with_tap(Configuration config, Board_Position tap, bool& failed) {
	config.taps[config.tap_count++] = tap;
	if (tap.has_up()) {
		if (config.board.is_gap(tap.get_up())) return with_move(config, tap);
	}
	if (tap.has_right()) {
		if (config.board.is_gap(tap.get_right())) return with_move(config, tap);
	}
	if (tap.has_down()) {
		if (config.board.is_gap(tap.get_down())) return with_move(config, tap);
	}
	if (tap.has_left()) {
		if (config.board.is_gap(tap.get_left())) return with_move(config, tap);
	}
	if (can_rotate(config.board[tap]) && get_rotations(config.board[tap]) < 3) {
		config.board.rotate(tap);
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
