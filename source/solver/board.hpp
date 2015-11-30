#ifndef BOARD
#define BOARD

#include <queue>

#include "tile.hpp"

const int tile_size = 32;

const int level_tile_width = 4;
const int level_tile_height = 6;

const int level_pixel_width = level_tile_width * tile_size;
const int level_pixel_height = level_tile_height * tile_size;

const int board_size = level_tile_width * level_tile_height;

typedef Tile Board[board_size];

typedef uint8_t Board_Position;

inline Board_Position board_position(int tile_x, int tile_y) { return tile_y * level_tile_width + tile_x; }

inline int get_x(Board_Position position) { return position % level_tile_width; }
inline int get_y(Board_Position position) {	return position / level_tile_width; }

inline bool has_up(Board_Position position) { return get_y(position) > 0; }
inline bool has_right(Board_Position position) { return get_x(position) < level_tile_width - 1; }
inline bool has_down(Board_Position position) { return get_y(position) < level_tile_height - 1; }
inline bool has_left(Board_Position position) { return get_x(position) > 0; }

inline Board_Position get_up(Board_Position position) { return board_position(get_x(position), get_y(position)-1); }
inline Board_Position get_right(Board_Position position) { return board_position(get_x(position)+1, get_y(position)); }
inline Board_Position get_down(Board_Position position) { return board_position(get_x(position), get_y(position)+1); }
inline Board_Position get_left(Board_Position position) { return board_position(get_x(position)-1, get_y(position)); }

bool boards_are_equal(const Board &one, const Board &two) {
	for (Board_Position position = 0; position < board_size; ++position)
		if (get_normalized(one[position]) != get_normalized(two[position]))
			return false;
	return true;
}

struct Board_State {
	bool filled[board_size];
	bool solved;
};

void reset_board_state(Board_State &state) {
	for (Board_Position position = 0; position < board_size; ++position)
		state.filled[position] = false;
	state.solved = false;
}

void update_board_state(const Board &board, Board_State &state) {
	reset_board_state(state);
	bool closed[board_size];
	for (Board_Position position = 0; position < board_size; ++position) closed[position] = false;
	std::queue<Board_Position> open;
	for (Board_Position position = 0; position < board_size; ++position)
		if (is_start(board[position]))
			open.push(position);
	while (!open.empty()) {
		Board_Position current = open.front(); open.pop();
		if (closed[current]) continue;
		closed[current] = true;
		state.filled[current] = true;
		if (is_end(board[current])) state.solved = true;
		if (has_up(current)) {
			Board_Position up = get_up(current);
			if (!closed[up] && connects_up(board[current], board[up])) open.push(up);
		}
		if (has_right(current)) {
			Board_Position right = get_right(current);
			if (!closed[right] && connects_right(board[current], board[right])) open.push(right);
		}
		if (has_down(current)) {
			Board_Position down = get_down(current);
			if (!closed[down] && connects_down(board[current], board[down])) open.push(down);
		}
		if (has_left(current)) {
			Board_Position left = get_left(current);
			if (!closed[left] && connects_left(board[current], board[left])) open.push(left);
		}
	}
}

#endif