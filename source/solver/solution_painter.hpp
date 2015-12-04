#ifndef SOLUTION_PAINTER
#define SOLUTION_PAINTER

#include <vector>

#include "../rgb_image.hpp"
#include "board.hpp"
#include "configuration.hpp"

class Solution_Painter {
public:
	Solution_Painter(Configuration level, std::vector<Configuration> &list, RGB_Image **reference_images);
	RGB_Image paint(bool &error);
private:
	int list_pixel_width();
	int list_pixel_height();
	int solution_pixel_width(Configuration solution);
	void paint_list(RGB_Image &destination);
	void paint_solution(RGB_Image &destination, Configuration level, Configuration solution);
	RGB_Image paint_board_at_offset(RGB_Image &destination, Board &board, Board_State &state, int offset);
	void paint_board(RGB_Image &destination, Board &board, Board_State &state);
private:
	bool *error;
	Configuration level;
	std::vector<Configuration> &list;
	RGB_Image **reference_images;
};

#endif