#include "stupid_queue.hpp"

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
	Stupid_Queue<Board_Position, board_size> open;
	for (Board_Position position = 0; position < board_size; ++position) {
		if (is_start(board[position])) {
			state.filled[position] = true;
			open.push(position);
			break;
		}
	}
	while (!open.empty()) {
		Board_Position current = open.pop();
		if (is_end(board[current])) state.solved = true;
		if (current.has_up()) {
			Board_Position up = current.get_up();
			if (!state.filled[up] && connects_up(board[current], board[up])) {
				state.filled[up] = true;
				open.push(up);
			}
		}
		if (current.has_right()) {
			Board_Position right = current.get_right();
			if (!state.filled[right] && connects_right(board[current], board[right])) {
				state.filled[right] = true;
				open.push(right);
			}
		}
		if (current.has_down()) {
			Board_Position down = current.get_down();
			if (!state.filled[down] && connects_down(board[current], board[down])) {
				state.filled[down] = true;
				open.push(down);
			}
		}
		if (current.has_left()) {
			Board_Position left = current.get_left();
			if (!state.filled[left] && connects_left(board[current], board[left])) {
				state.filled[left] = true;
				open.push(left);
			}
		}
	}
}