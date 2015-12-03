#ifndef SOLUTION_LIST
#define SOLUTION_LIST

#include <cstdint>
#include <vector>

#include "configuration.hpp"

class Solution_List {
public:
	Solution_List() : found(false) {}
	bool should_skip(const Configuration &config);
	void append(const Configuration &config);
	std::vector<Configuration> get() { return solutions; }
private:
	void push_first(const Configuration &config);
	bool contains(const Configuration &config);
private:
	bool found;
	std::uint8_t tap_count;
	std::uint8_t action_count;
	std::vector<Configuration> solutions;
};

#endif