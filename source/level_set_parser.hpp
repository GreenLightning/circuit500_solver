#ifndef LEVEL_SET_PARSER
#define LEVEL_SET_PARSER

#include <vector>
#include <string>

#include <boost/regex.hpp>
#include <boost/icl/interval_set.hpp>

namespace icl = boost::icl;

class Level_Set_Parser {
public:
	Level_Set_Parser(const std::vector<std::string> &input_list);
	icl::interval_set<int> parse();

private:
	void parse_input(const std::string input);
	void parse_level_number(std::string level_text);
	void parse_level_interval(std::string low_text, std::string high_text);
	
	const std::vector<std::string> &input_list;
	const boost::regex regex;
	icl::interval_set<int> level_set; 
};

#endif