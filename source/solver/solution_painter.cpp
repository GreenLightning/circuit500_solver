#include <vector>
#include <algorithm>

#include "../rgb_image.hpp"
#include "board.hpp"
#include "reference.hpp"
#include "configuration.hpp"

#include "solution_painter.hpp"

int solution_list_pixel_width(std::vector<Configuration> &list) {
	int width = 0;
	for(auto solution = list.begin(); solution != list.end(); ++solution)
		width = std::max(width, solution_pixel_width(*solution));
	return width;
}

int solution_list_pixel_height(std::vector<Configuration> &list) {
	return list.size() * (level_pixel_height + tile_size) - tile_size;
}

void draw_solution_list(RGB_Image &destination, Configuration level, std::vector<Configuration> &list, RGB_Image **reference_images) {
	bool error = false;
	int offset = 0;
	for (auto solution = list.begin(); solution != list.end(); ++solution, ++offset) {
		RGB_Image img = rgb_image_create_view(destination, 0, offset * (level_pixel_height + tile_size), destination.getWidth(), level_pixel_height, error);
		draw_solution(img, level, *solution, reference_images);
	}
}

int solution_pixel_width(Configuration solution) {
	return solution.tap_count * (level_pixel_width + tile_size) + level_pixel_width;
}

void draw_solution(RGB_Image &destination, Configuration level, Configuration solution, RGB_Image **reference_images) {
	Board_State state;
	bool error = false;
	int offset = 0;
	for (int step = 0; step < solution.tap_count; ++step, ++offset) {
		RGB_Image img = rgb_image_create_view(destination, offset * (level_pixel_width + tile_size), 0, level_pixel_width, destination.getHeight(), error);
		update_board_state(level.board, state);
		draw_board(img, level.board, state, reference_images);
		Board_Position tap = solution.taps[step];
		RGB_Image tap_img = rgb_image_create_view(img, get_x(tap) * tile_size, get_y(tap) * tile_size, tile_size, tile_size, error);
		rgb_image_tint(tap_img, 0x80ff0000, error);
		level = with_tap(level, tap);
	}
	RGB_Image img = rgb_image_create_view(destination, offset * (level_pixel_width + tile_size), 0, level_pixel_width, destination.getHeight(), error);
	update_board_state(level.board, state);
	draw_board(img, level.board, state, reference_images);
}

void draw_board(RGB_Image &destination, Board &board, Board_State &state, RGB_Image **reference_images) {
	bool error = false;
	for (int y = 0; y < level_tile_height; ++y) {
		for (int x = 0; x < level_tile_width; ++x) {
			Board_Position position = board_position(x, y);
			RGB_Image dst_tile = rgb_image_create_view(destination, x * tile_size, y * tile_size, tile_size, tile_size, error);
			RGB_Image &ref_img = *reference_images[get_reference_index(board[position], state.filled[position])];
			rgb_image_copy_from_to(ref_img, dst_tile, error);
		}
	}
}