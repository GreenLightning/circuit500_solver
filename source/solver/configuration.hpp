#ifndef CONFIGURATION
#define CONFIGURATION

#include "tile.hpp"
#include "board.hpp"

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

#endif