#ifndef REFERENCE
#define REFERENCE

#include <iostream>
#include <sstream>
#include <string>

#include "../rgb_image.hpp"
#include "tile.hpp"

const int number_of_references = 32;

const std::string reference_names[number_of_references] = {
	"gap",
	"empty",
	"corner_0",
	"corner_1",
	"corner_2",
	"corner_3",
	"junction_0",
	"junction_1",
	"junction_2",
	"junction_3",
	"cross",
	"end_0",
	"end_1",
	"end_2",
	"end_3",
	"start_0",
	"start_1",
	"start_2",
	"start_3",
	"corner_filled_0",
	"corner_filled_1",
	"corner_filled_2",
	"corner_filled_3",
	"junction_filled_0",
	"junction_filled_1",
	"junction_filled_2",
	"junction_filled_3",
	"cross_filled",
	"end_filled_0",
	"end_filled_1",
	"end_filled_2",
	"end_filled_3"
};

const Tile reference_tiles[number_of_references] = {
	create_gap(),
	create_tile(false, false, false, false),
	create_tile(true, true, false, false),
	create_tile(false, true, true, false),
	create_tile(false, false, true, true),
	create_tile(true, false, false, true),
	create_tile(false, true, true, true),
	create_tile(true, false, true, true),
	create_tile(true, true, false, true),
	create_tile(true, true, true, false),
	create_tile(true, true, true, true),
	create_end(tile_direction_up),
	create_end(tile_direction_right),
	create_end(tile_direction_down),
	create_end(tile_direction_left),
	create_start(tile_direction_up),
	create_start(tile_direction_right),
	create_start(tile_direction_down),
	create_start(tile_direction_left),
	create_tile(true, true, false, false),
	create_tile(false, true, true, false),
	create_tile(false, false, true, true),
	create_tile(true, false, false, true),
	create_tile(false, true, true, true),
	create_tile(true, false, true, true),
	create_tile(true, true, false, true),
	create_tile(true, true, true, false),
	create_tile(true, true, true, true),
	create_end(tile_direction_up),
	create_end(tile_direction_right),
	create_end(tile_direction_down),
	create_end(tile_direction_left)
};

int get_reference_index(Tile tile, bool filled) {
	Tile normal = get_normalized(tile);
	if (!filled) {
		for (int i = 0; i < 15; ++i)
			if (normal == reference_tiles[i])
				return i;
	} else {
		for (int i = 15; i < number_of_references; ++i)
			if (normal == reference_tiles[i])
				return i;
	}
	return -1;
}

RGB_Image **load_reference_images(bool &error) {
	if (error) return nullptr;
	RGB_Image **reference_images = new RGB_Image*[number_of_references];
	if (reference_images == nullptr) {
		std::cout << "Error: Out of memory while allocating reference array." << std::endl;
		error = true;
		return nullptr;
	}
	for (int i = 0; i < number_of_references; ++i) {
		std::ostringstream filename;
		filename << "data/reference/tile_" << reference_names[i] << ".png";
		reference_images[i] = new RGB_Image(rgb_image_load(filename.str(), tile_size, tile_size, error));
	}
	return reference_images;
}

void free_reference_images(RGB_Image **reference_images) {
	delete [] reference_images;
}

#endif