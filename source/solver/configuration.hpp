#pragma once

#include <cstdint>

#include "board.hpp"
#include "tile.hpp"

constexpr int maximum_number_of_taps = 16;

class Configuration {
public:
	Configuration() : taps{0}, tap_count(0), action_count(0) {}
public:
	Board board;
	Board_Position taps[maximum_number_of_taps];
	uint8_t tap_count;
	uint8_t action_count;
};

Configuration with_tap(Configuration config, Board_Position tap, bool& failed);
Configuration with_tap(Configuration config, Board_Position tap);
