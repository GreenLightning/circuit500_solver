#pragma once

#include "../rgb_image.hpp"
#include "tile.hpp"

constexpr int number_of_references = 32;

class Reference_Info {
public:
	constexpr Reference_Info(const char* name, Tile tile, bool start, bool end, bool gap, bool filled)
		: name(name), tile(tile), start(start), end(end), gap(gap), filled(filled) {}

	const char* name;
	Tile tile;
	bool start, end, gap, filled;
};

constexpr Reference_Info reference_infos[number_of_references] = {
	{ "gap",               create_tile(false, false, false, false), false, false, true,  false },
	{ "empty",             create_tile(false, false, false, false), false, false, false, false },
	{ "corner_0",          create_tile(true, true, false, false),   false, false, false, false },
	{ "corner_1",          create_tile(false, true, true, false),   false, false, false, false },
	{ "corner_2",          create_tile(false, false, true, true),   false, false, false, false },
	{ "corner_3",          create_tile(true, false, false, true),   false, false, false, false },
	{ "junction_0",        create_tile(false, true, true, true),    false, false, false, false },
	{ "junction_1",        create_tile(true, false, true, true),    false, false, false, false },
	{ "junction_2",        create_tile(true, true, false, true),    false, false, false, false },
	{ "junction_3",        create_tile(true, true, true, false),    false, false, false, false },
	{ "cross",             create_tile(true, true, true, true),     false, false, false, false },
	{ "end_0",             tile_direction_up,                       false, true,  false, false },
	{ "end_1",             tile_direction_right,                    false, true,  false, false },
	{ "end_2",             tile_direction_down,                     false, true,  false, false },
	{ "end_3",             tile_direction_left,                     false, true,  false, false },
	{ "start_0",           tile_direction_up,                       true,  false, false, true  },
	{ "start_1",           tile_direction_right,                    true,  false, false, true  },
	{ "start_2",           tile_direction_down,                     true,  false, false, true  },
	{ "start_3",           tile_direction_left,                     true,  false, false, true  },
	{ "corner_filled_0",   create_tile(true, true, false, false),   false, false, false, true  },
	{ "corner_filled_1",   create_tile(false, true, true, false),   false, false, false, true  },
	{ "corner_filled_2",   create_tile(false, false, true, true),   false, false, false, true  },
	{ "corner_filled_3",   create_tile(true, false, false, true),   false, false, false, true  },
	{ "junction_filled_0", create_tile(false, true, true, true),    false, false, false, true  },
	{ "junction_filled_1", create_tile(true, false, true, true),    false, false, false, true  },
	{ "junction_filled_2", create_tile(true, true, false, true),    false, false, false, true  },
	{ "junction_filled_3", create_tile(true, true, true, false),    false, false, false, true  },
	{ "cross_filled",      create_tile(true, true, true, true),     false, false, false, true  },
	{ "end_filled_0",      tile_direction_up,                       false, true,  false, true  },
	{ "end_filled_1",      tile_direction_right,                    false, true,  false, true  },
	{ "end_filled_2",      tile_direction_down,                     false, true,  false, true  },
	{ "end_filled_3",      tile_direction_left,                     false, true,  false, true  }
};

int get_reference_index(Tile tile, bool start, bool end, bool gap, bool filled);
RGB_Image** load_reference_images(bool& error);
void free_reference_images(RGB_Image** reference_images);
