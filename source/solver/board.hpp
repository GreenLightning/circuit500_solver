#ifndef BOARD
#define BOARD

#include "sizes.hpp"
#include "tile.hpp"

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

bool boards_are_equal(const Board &one, const Board &two);

struct Board_State {
	bool filled[board_size];
	bool solved;
};

void reset_board_state(Board_State &state);
void update_board_state(const Board &board, Board_State &state);

#endif