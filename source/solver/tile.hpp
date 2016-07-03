#pragma once

#include <cstdint>

/* tile format
 *
 * bit number: 76543210
 *        bit: URDLNNCC
 *
 * U = up
 * R = right
 * D = down
 * L = left
 * N = not used
 * C = rotation count
 */

typedef uint8_t Tile;

constexpr Tile tile_rotation_bits = 0b00000011;

enum Tile_Direction : Tile {
	tile_direction_up    = 0b10000000,
	tile_direction_right = 0b01000000,
	tile_direction_down  = 0b00100000,
	tile_direction_left  = 0b00010000
};

constexpr Tile create_tile(bool up, bool right, bool down, bool left) {
	return (up    ? tile_direction_up    : 0)
	     | (right ? tile_direction_right : 0)
	     | (down  ? tile_direction_down  : 0)
	     | (left  ? tile_direction_left  : 0);
}

inline bool can_rotate(Tile t) { int matrix = t >> 4; return matrix != 0b0000 && matrix != 0b1111; }
inline bool cannot_rotate(Tile t) { int matrix = t >> 4; return matrix == 0b0000 || matrix == 0b1111; }

inline int get_rotations(Tile t) { return t & tile_rotation_bits; }

inline Tile get_rotated(Tile t) {
	return ((t << 3) & 0b10000000)
	     | ((t >> 1) & 0b01110000)
	     | ((t + 1) & tile_rotation_bits);
}

inline Tile get_normalized(Tile t) {
	return (t & ~tile_rotation_bits);
}

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
