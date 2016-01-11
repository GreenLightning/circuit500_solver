#pragma once

#include <cstdint>

#include "board.hpp"
#include "tile.hpp"

constexpr int maximum_number_of_taps = 16;

struct Configuration {
	Board board;
	Board_Position taps[maximum_number_of_taps];
	std::uint8_t tap_count;
	std::uint8_t action_count;
};

Configuration with_tap(Configuration config, Board_Position tap, bool &failed);
Configuration with_tap(Configuration config, Board_Position tap);