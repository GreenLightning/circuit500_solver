#ifndef TILE
#define TILE

#include <cstdint>

/* tile format
 *
 * bit number: 76543210
 *  bit value: 1
 *             2631
 *             84268421
 *        bit: URDLTTCC
 *
 * U = up
 * R = right
 * D = down
 * L = left
 * T = type
 * C = rotation count
 *
 * type table
 * 
 * 00 = gap
 * 10 = start
 * 01 = end
 * 11 = other (empty or normal pipes)
 */

typedef std::uint8_t Tile;

constexpr Tile tile_type_bits  = 0b00001100;
constexpr Tile tile_rotation_bits = 0b00000011;

enum Tile_Direction : Tile {
	tile_direction_up = 128,
	tile_direction_right = 64,
	tile_direction_down = 32,
	tile_direction_left = 16
};

enum Tile_Type : Tile {
	tile_type_gap = 0b0000,
	tile_type_start = 0b1000,
	tile_type_end = 0b0100,
	tile_type_other = 0b1100
};

constexpr Tile create_gap() {
	return tile_type_gap; 
}

constexpr Tile create_start(Tile_Direction dir) {
	return dir | tile_type_start;
}

constexpr Tile create_end(Tile_Direction dir) {
	return dir | tile_type_end;
}

constexpr Tile create_tile(bool up, bool right, bool down, bool left) {
	return (up ? tile_direction_up : 0)
	     | (right ? tile_direction_right : 0)
	     | (down ? tile_direction_down : 0)
	     | (left ? tile_direction_left : 0)
	     | tile_type_other;
}

inline bool can_rotate(Tile t) { int matrix = t >> 4; return matrix != 0b0000 && matrix != 0b1111; }
inline bool cannot_rotate(Tile t) { int matrix = t >> 4; return matrix == 0b0000 || matrix == 0b1111; }

inline int get_rotations(Tile t) { return t & tile_rotation_bits; }

inline Tile get_rotated(Tile t) {
	return ((t << 3) & 0b10000000) | ((t >> 1) & 0b01110000) | (t & tile_type_bits) | ((t + 1) & tile_rotation_bits);
}

inline Tile get_normalized(Tile t) {
	return (t & ~tile_rotation_bits);
}

inline bool is_gap(Tile t)   { return (t & tile_type_bits) == tile_type_gap;   }
inline bool is_start(Tile t) { return (t & tile_type_bits) == tile_type_start; }
inline bool is_end(Tile t)   { return (t & tile_type_bits) == tile_type_end;   }
inline bool is_other(Tile t) { return (t & tile_type_bits) == tile_type_other; }

inline bool connects(Tile t, Tile_Direction dir) { return t & dir; }

inline bool connects_up(Tile t, Tile partner) {
	return connects(t, tile_direction_up) && connects(partner, tile_direction_down);
}

inline bool connects_right(Tile t, Tile partner) {
	return connects(t, tile_direction_right) && connects(partner, tile_direction_left);
}

inline bool connects_down(Tile t, Tile partner) {
	return connects(t, tile_direction_down) && connects(partner, tile_direction_up);
}

inline bool connects_left(Tile t, Tile partner) {
	return connects(t, tile_direction_left) && connects(partner, tile_direction_right);
}

#endif