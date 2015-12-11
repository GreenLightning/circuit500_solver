#include <iostream>
#include <sstream>
#include <string>

#include "../rgb_image.hpp"
#include "sizes.hpp"
#include "tile.hpp"

#include "reference.hpp"

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
		reference_images[i] = new RGB_Image(RGB_Image::load(filename.str(), tile_size, tile_size, error));
	}
	return reference_images;
}

void free_reference_images(RGB_Image **reference_images) {
	delete [] reference_images;
}