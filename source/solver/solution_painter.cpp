#include <algorithm>
#include <vector>

#include "../rgb_image.hpp"
#include "board.hpp"
#include "configuration.hpp"
#include "reference.hpp"

#include "solution_painter.hpp"

constexpr int painter_border = tile_size;
constexpr int painter_spacing = tile_size;

Solution_Painter::Solution_Painter(Configuration level, const std::vector<Configuration>& list, RGB_Image** reference_images)
	: level(level), list(list), reference_images(reference_images) {}

RGB_Image Solution_Painter::paint(bool& error) {
	this->error = &error;
	int list_width = list_pixel_width();
	int list_height = list_pixel_height();
	int image_width = list_width + 2 * painter_border;
	int image_height = list_height + 2 * painter_border;
	RGB_Image image = RGB_Image::create(image_width, image_height, 0x000000, error);
	RGB_Image list_view = image.create_view(painter_border, painter_border, list_width, list_height, error);
	paint_list(list_view);
	return image;
}

int Solution_Painter::list_pixel_width() {
	int width = 0;
	for(auto&& solution : list)
		width = std::max(width, solution_pixel_width(solution));
	return width;
}

int Solution_Painter::list_pixel_height() {
	return list.size() * (level_pixel_height + painter_spacing) - painter_spacing;
}

int Solution_Painter::solution_pixel_width(Configuration solution) {
	return solution.tap_count * (level_pixel_width + painter_spacing) + level_pixel_width;
}

void Solution_Painter::paint_list(RGB_Image& destination) {
	if (*error) return;
	int offset = 0;
	for (auto solution = list.begin(); solution != list.end(); ++solution, ++offset) {
		int x = 0, y = offset * (level_pixel_height + painter_spacing);
		int width = destination.getWidth(), height = level_pixel_height;
		RGB_Image solution_view = destination.create_view(0, y, width, height, *error);
		paint_solution(solution_view, level, *solution);
	}
}

void Solution_Painter::paint_solution(RGB_Image& destination, Configuration level, Configuration solution) {
	if (*error) return;
	Board_State state;
	int position = 0;
	for (; position < solution.tap_count; ++position) {
		RGB_Image board_view = paint_board_at_offset(destination, level.board, state, position);
		Board_Position tap = solution.taps[position];
		int x = tap.get_tile_x() * tile_size, y = tap.get_tile_y() * tile_size;
		RGB_Image tap_view = board_view.create_view(x, y, tile_size, tile_size, *error);
		tap_view.tint(0x80ff0000, *error);
		level = with_tap(level, tap).config;
	}
	paint_board_at_offset(destination, level.board, state, position);
}

RGB_Image Solution_Painter::paint_board_at_offset(RGB_Image& destination, Board& board, Board_State& state, int offset) {
	state.update(board);
	int x = offset * (level_pixel_width + painter_spacing), y = 0;
	int width = level_pixel_width, height = destination.getHeight();
	RGB_Image board_view = destination.create_view(x, y, width, height, *error);
	paint_board(board_view, board, state);
	return board_view;
}

void Solution_Painter::paint_board(RGB_Image& destination, Board& board, Board_State& state) {
	for (int y = 0; y < level_tile_height; ++y) {
		for (int x = 0; x < level_tile_width; ++x) {
			Board_Position position(x, y);
			RGB_Image tile_view = destination.create_view(x * tile_size, y * tile_size, tile_size, tile_size, *error);
			bool start = board.is_start(position);
			bool end = board.is_end(position);
			bool gap = board.is_gap(position);
			bool filled = state.is_filled(position);
			RGB_Image& ref_img = *reference_images[get_reference_index(board[position], start, end, gap, filled)];
			ref_img.copy_to(tile_view, *error);
		}
	}
}
