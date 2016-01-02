#ifndef CONFIGURATION
#define CONFIGURATION

#include <cstdint>

#include "board.hpp"
#include "tile.hpp"

const int maximum_number_of_taps = 6;

struct Configuration {
	Board board;
	Board_Position taps[maximum_number_of_taps];
	std::uint8_t tap_count;
	std::uint8_t action_count;
};

Configuration with_tap(Configuration config, Board_Position tap, bool &failed);
Configuration with_tap(Configuration config, Board_Position tap);

#endif