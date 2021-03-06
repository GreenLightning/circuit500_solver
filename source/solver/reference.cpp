#include <iostream>
#include <sstream>
#include <string>

#include "../rgb_image.hpp"
#include "sizes.hpp"
#include "tile.hpp"

#include "reference.hpp"

int get_reference_index(Tile tile, bool start, bool end, bool gap, bool filled) {
	Tile normal = get_normalized(tile);
	for (int i = 0; i < number_of_references; ++i) {
		const Reference_Info& info = reference_infos[i];
		if (info.tile == normal
			&& info.start == start
			&& info.end == end
			&& info.gap == gap
			&& info.filled == filled)
			return i;
	}
	return -1;
}

RGB_Image** load_reference_images(bool& error) {
	if (error) return nullptr;
	RGB_Image** reference_images = new RGB_Image*[number_of_references];
	if (reference_images == nullptr) {
		std::cout << "Error: Out of memory while allocating reference array." << std::endl;
		error = true;
		return nullptr;
	}
	for (int i = 0; i < number_of_references; ++i) {
		std::ostringstream filename;
		filename << "data/reference/tile_" << reference_infos[i].name << ".png";
		reference_images[i] = new RGB_Image(RGB_Image::load(filename.str(), tile_size, tile_size, error));
	}
	return reference_images;
}

void free_reference_images(RGB_Image** reference_images) {
	delete [] reference_images;
}
