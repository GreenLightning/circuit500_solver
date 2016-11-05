#pragma once

#include <regex>
#include <string>
#include <vector>

#include "level_set.hpp"

class Level_Set_Parser {
public:
	Level_Set_Parser(const std::vector<std::string>& input_list, Level_Set& level_set);
	void parse();

private:
	void parse_input(const std::string input);
	void parse_level_number(std::string level_text);
	void parse_level_interval(std::string low_text, std::string high_text);
	
	const std::vector<std::string>& input_list;
	const std::regex regex;
	Level_Set& level_set;
};
