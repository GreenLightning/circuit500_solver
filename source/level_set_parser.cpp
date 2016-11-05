#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "level_set_parser.hpp"

Level_Set_Parser::Level_Set_Parser(const std::vector<std::string>& input_list, Level_Set& level_set) :
	input_list(input_list),
	regex("(\\d{1,3})(?:-(\\d{1,3}))?"),
	level_set(level_set) {}

void Level_Set_Parser::parse() {
	for (std::string input : input_list)
		parse_input(input);
}

void Level_Set_Parser::parse_input(std::string input) {
	std::smatch match;

	if (!regex_match(input, match, regex))
		throw std::runtime_error("expected level number or level range, but found: '" + input + "'");

	if (match[2].matched)
		parse_level_interval(match[1], match[2]);
	else
		parse_level_number(match[1]);
}

void Level_Set_Parser::parse_level_number(std::string level_text) {
	int level = std::stoi(level_text);
	if (level < 1)
		throw std::runtime_error("level number must be greater than 0, but was: '" + level_text + "'");
	if (level > 500)
		throw std::runtime_error("level number must be 500 or less, but was: '" + level_text + "'");
	level_set.set(level);
}

void Level_Set_Parser::parse_level_interval(std::string low_text, std::string high_text) {
	int low = std::stoi(low_text);
	int high = std::stoi(high_text);
	if (low > high)
		throw std::runtime_error("must specify smaller level number first: '" + low_text + "-" + high_text + "'");
	if (low < 1)
		throw std::runtime_error("level numbers must be greater than 0: '" + low_text + "-" + high_text + "'");
	if (high > 500)
		throw std::runtime_error("level numbers must be 500 or less: '" + low_text + "-" + high_text + "'");
	level_set.set_inclusive(low, high);
}
