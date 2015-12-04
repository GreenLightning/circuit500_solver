#include <vector>
#include <algorithm>

#include "../rgb_image.hpp"
#include "board.hpp"
#include "reference.hpp"
#include "configuration.hpp"

#include "solution_painter.hpp"

const int painter_border = tile_size;
const int painter_spacing = tile_size;

Solution_Painter::Solution_Painter(Configuration level, std::vector<Configuration> &list, RGB_Image **reference_images)
	: level(level), list(list), reference_images(reference_images) {}

RGB_Image Solution_Painter::paint(bool &error) {
	this->error = &error;
	int list_width = list_pixel_width();
	int list_height = list_pixel_height();
	int image_width = list_width + 2 * painter_border;
	int image_height = list_height + 2 * painter_border;
	RGB_Image image = rgb_image_create(image_width, image_height, 0x000000, error);
	paint_list(rgb_image_create_view(image, painter_border, painter_border, list_width, list_height, error));
	return image;
}

int Solution_Painter::list_pixel_width() {
	int width = 0;
	for(auto &&solution : list)
		width = std::max(width, solution_pixel_width(solution));
	return width;
}

int Solution_Painter::list_pixel_height() {
	return list.size() * (level_pixel_height + painter_spacing) - painter_spacing;
}

int Solution_Painter::solution_pixel_width(Configuration solution) {
	return solution.tap_count * (level_pixel_width + painter_spacing) + level_pixel_width;
}

void Solution_Painter::paint_list(RGB_Image &destination) {
	if (*error) return;
	int offset = 0;
	for (auto solution = list.begin(); solution != list.end(); ++solution, ++offset) {
		int x = 0, y = offset * (level_pixel_height + painter_spacing);
		int width = destination.getWidth(), height = level_pixel_height;
		RGB_Image solution_view = rgb_image_create_view(destination, 0, y, width, height, *error);
		paint_solution(solution_view, level, *solution);
	}
}

void Solution_Painter::paint_solution(RGB_Image &destination, Configuration level, Configuration solution) {
	if (*error) return;
	Board_State state;
	int position = 0;
	for (; position < solution.tap_count; ++position) {
		RGB_Image board_view = paint_board_at_offset(destination, level.board, state, position);
		Board_Position tap = solution.taps[position];
		int x = get_x(tap) * tile_size, y = get_y(tap) * tile_size;
		RGB_Image tap_view = rgb_image_create_view(board_view, x, y, tile_size, tile_size, *error);
		rgb_image_tint(tap_view, 0x80ff0000, *error);
		level = with_tap(level, tap);
	}
	paint_board_at_offset(destination, level.board, state, position);
}

RGB_Image Solution_Painter::paint_board_at_offset(RGB_Image &destination, Board &board, Board_State &state, int offset) {
	update_board_state(board, state);
	int x = offset * (level_pixel_width + painter_spacing), y = 0;
	int width = level_pixel_width, height = destination.getHeight();
	RGB_Image board_view = rgb_image_create_view(destination, x, y, width, height, *error);
	paint_board(board_view, board, state);
	return board_view;
}

void Solution_Painter::paint_board(RGB_Image &destination, Board &board, Board_State &state) {
	for (int y = 0; y < level_tile_height; ++y) {
		for (int x = 0; x < level_tile_width; ++x) {
			Board_Position position = board_position(x, y);
			RGB_Image tile_view = rgb_image_create_view(destination, x * tile_size, y * tile_size, tile_size, tile_size, *error);
			RGB_Image &ref_img = *reference_images[get_reference_index(board[position], state.filled[position])];
			rgb_image_copy_from_to(ref_img, tile_view, *error);
		}
	}
}