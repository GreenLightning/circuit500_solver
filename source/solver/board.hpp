#ifndef BOARD
#define BOARD

#include "sizes.hpp"
#include "tile.hpp"

const int board_size = level_tile_width * level_tile_height;

typedef Tile Board[board_size];

class Board_Position {
public:

	Board_Position()
		: index(0) {}

	Board_Position(uint8_t index)
		: index(index) {}

	Board_Position(int tile_x, int tile_y)
		: index(tile_y * level_tile_width + tile_x)	{}

	int get_tile_x() const {
		// return index % level_tile_width;
		// must be power of two
		return index & (level_tile_width - 1);
	}

	int get_tile_y() const {
		return index / level_tile_width;
	}

	bool has_up() const {
		// return get_tile_y() > 0;
		return index >= level_tile_width;
	}

	bool has_right() const {
		return get_tile_x() < level_tile_width - 1;
	}

	bool has_down() const {
		// return get_tile_y() < level_tile_height - 1;
		return index < (level_tile_height - 1) * level_tile_width;
	}

	bool has_left() const {
		return get_tile_x() > 0;
	}

	Board_Position get_up() const {
		return Board_Position(index - level_tile_width);
	}

	Board_Position get_right() const {
		return Board_Position(index + 1);
	}

	Board_Position get_down() const {
		return Board_Position(index + level_tile_width);
	}

	Board_Position get_left() const {
		return Board_Position(index - 1);
	}

	Board_Position& operator ++() {
		++index;
		return *this;
	}

	operator uint8_t() const {
		return index;
	}

private:
	uint8_t index;
};

bool boards_are_equal(const Board &one, const Board &two);

struct Board_State {
	bool filled[board_size];
	bool solved;
};

void reset_board_state(Board_State &state);
void update_board_state(const Board &board, Board_State &state);

#endif