#include "tile.hpp"
#include "board.hpp"

#include "configuration.hpp"

Configuration with_move(Configuration config, Board_Position tap_position, Board_Position gap_position) {
	Tile tap_tile = config.board[tap_position];
	config.board[tap_position] = config.board[gap_position];
	config.board[gap_position] = tap_tile;
	if (can_rotate(tap_tile)) ++config.action_count;
	return config;
}

Configuration with_tap(Configuration config, Board_Position tap, bool &failed) {
	config.taps[config.tap_count++] = tap;
	if (tap.has_up()) {
		Board_Position up = tap.get_up();
		if (is_gap(config.board[up])) return with_move(config, tap, up);
	}
	if (tap.has_right()) {
		Board_Position right = tap.get_right();
		if (is_gap(config.board[right])) return with_move(config, tap, right);
	}
	if (tap.has_down()) {
		Board_Position down = tap.get_down();
		if (is_gap(config.board[down])) return with_move(config, tap, down);
	}
	if (tap.has_left()) {
		Board_Position left = tap.get_left();
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