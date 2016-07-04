#include "stupid_queue.hpp"

#include "board.hpp"

bool operator==(const Board& one, const Board& two) {
	for (Board_Position position = 0; position < board_size; ++position)
		if (get_normalized(one[position]) != get_normalized(two[position]))
			return false;
	return one.start == two.start && one.end == two.end && one.gap == two.gap;
}

void Board_State::update(const Board& board) {
	reset();
	Stupid_Queue<Board_Position, board_size> open;
	open.push(board.get_start());
	set_filled(board.get_start());
	while (!open.empty()) {
		Board_Position current = open.pop();
		if (board.is_end(current)) set_solved();
		Tile currentTile = board[current];
		if (current.has_up()) {
			Board_Position up = current.get_up();
			if (connects_up(currentTile, board[up]) && !is_filled(up)) {
				open.push(up);
				set_filled(up);
			}
		}
		if (current.has_right()) {
			Board_Position right = current.get_right();
			if (connects_right(currentTile, board[right]) && !is_filled(right)) {
				open.push(right);
				set_filled(right);
			}
		}
		if (current.has_down()) {
			Board_Position down = current.get_down();
			if (connects_down(currentTile, board[down]) && !is_filled(down)) {
				open.push(down);
				set_filled(down);
			}
		}
		if (current.has_left()) {
			Board_Position left = current.get_left();
			if (connects_left(currentTile, board[left]) && !is_filled(left)) {
				open.push(left);
				set_filled(left);
			}
		}
	}
}
