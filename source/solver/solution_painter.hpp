#ifndef SOLUTION_PAINTER
#define SOLUTION_PAINTER

#include <vector>

#include "../rgb_image.hpp"
#include "board.hpp"
#include "configuration.hpp"

int solution_list_pixel_width(std::vector<Configuration> &list);
int solution_list_pixel_height(std::vector<Configuration> &list);
void draw_solution_list(RGB_Image &destination, Configuration level, std::vector<Configuration> &list, RGB_Image **reference_images);
int solution_pixel_width(Configuration solution);
void draw_solution(RGB_Image &destination, Configuration level, Configuration solution, RGB_Image **reference_images);
void draw_board(RGB_Image &destination, Board &board, Board_State &state, RGB_Image **reference_images);

#endif