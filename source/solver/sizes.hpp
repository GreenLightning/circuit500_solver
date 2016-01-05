#ifndef SIZES
#define SIZES

constexpr int tile_size = 32;

constexpr int level_tile_width = 4;
constexpr int level_tile_height = 6;

constexpr int level_pixel_width = level_tile_width * tile_size;
constexpr int level_pixel_height = level_tile_height * tile_size;

#endif