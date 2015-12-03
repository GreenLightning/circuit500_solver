#include <cstdint>

#include "tile.hpp"

Tile create_gap() {
	return tile_type_gap; 
}

Tile create_start(Tile_Direction dir) {
	return dir | tile_type_start;
}

Tile create_end(Tile_Direction dir) {
	return dir | tile_type_end;
}

Tile create_tile(bool up, bool right, bool down, bool left) {
	return (up ? tile_direction_up : 0)
	     | (right ? tile_direction_right : 0)
	     | (down ? tile_direction_down : 0)
	     | (left ? tile_direction_left : 0)
	     | tile_type_other;
}