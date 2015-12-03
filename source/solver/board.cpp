#include <queue>

#include "board.hpp"

bool boards_are_equal(const Board &one, const Board &two) {
	for (Board_Position position = 0; position < board_size; ++position)
		if (get_normalized(one[position]) != get_normalized(two[position]))
			return false;
	return true;
}

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