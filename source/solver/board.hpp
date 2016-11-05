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

	Board_Position(uint_fast8_t index)
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

	uint_fast32_t get_mask() const {
		return uint_fast32_t(1) << index;
	}

	Board_Position& operator++() {
		++index;
		return *this;
	}

	operator uint_fast8_t() const {
		return index;
	}

private:
	uint_fast8_t index;
};

class Board {
public:
	Board() : start(255), end(255), gap(255) {}
	Tile operator[](Board_Position position) const { return data[position.index]; }
	friend bool operator==(const Board& one, const Board& two);

	bool has_gap() const { return gap < board_size; }

	Board_Position get_start() const { return start; }
	Board_Position get_gap() const { return gap; }

	bool is_start(Board_Position position) const { return position == start; }
	bool is_end(Board_Position position) const { return position == end; }
	bool is_gap(Board_Position position) const { return position == gap; }

	void swap_with_gap(Board_Position position) {
		Tile tmp = data[position];
		data[position] = data[gap];
		data[gap] = tmp;
		if (position == start) start = gap;
		if (position == end) end = gap;
		gap = position;
	}

	void rotate(Board_Position position) {
		data[position] = get_rotated(data[position]);
	}

	void set(Board_Position position, Tile tile, bool is_start, bool is_end, bool is_gap) {
		data[position] = tile;
		if (is_start) start = position;
		if (is_end) end = position;
		if (is_gap) gap = position;
	}

private:
	Tile data[board_size];
	Board_Position start, end, gap;
};

class Board_State {

private:
	static constexpr uint_fast32_t SOLVED_FLAG = uint32_t(1) << 31;

public:
	void update(const Board& board);
	bool is_solved() { return state & SOLVED_FLAG; }
	bool is_filled(Board_Position position) { return state & position.get_mask(); }

private:
	void reset() { state = 0; }
	void set_solved() { state |= SOLVED_FLAG; }
	void set_filled(Board_Position position) { state |= position.get_mask(); }

private:
	uint_fast32_t state;
};
