#pragma once

#include <cstdint>

#include "sizes.hpp"
#include "tile.hpp"

constexpr int board_size = level_tile_width * level_tile_height;

class Board_Position {
	friend class Board;
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

	uint32_t get_mask() const {
		return uint32_t(1) << index;
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

class Board {
public:
	Tile& operator[](Board_Position position) { return data[position.index]; }
	const Tile& operator[](Board_Position position) const { return data[position.index]; }
	friend bool operator==(const Board& one, const Board& two);
private:
	Tile data[board_size];
};

class Board_State {

private:
	static constexpr uint32_t SOLVED_FLAG = uint32_t(1) << 31;

public:
	void update(const Board& board);
	bool is_solved() { return state & SOLVED_FLAG; }
	bool is_filled(Board_Position position) { return state & position.get_mask(); }

private:
	void reset() { state = 0; }
	void set_solved() { state |= SOLVED_FLAG; }
	void set_filled(Board_Position position) { state |= position.get_mask(); }

private:
	uint32_t state;
};
