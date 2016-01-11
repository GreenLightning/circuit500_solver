#pragma once

#include <cstdint>
#include <vector>

#include "configuration.hpp"

class Solution_List {
public:
	Solution_List() : found(false) {}
	bool should_skip(const Configuration &config);
	void append(const Configuration &config);
	bool empty() { return !found; }
	std::vector<Configuration> get_solutions() { return solutions; }
	int get_tap_count() { return found ? tap_count : -1; }
	int get_action_count() { return found ? action_count : -1; }
private:
	void push_first(const Configuration &config);
	bool contains(const Configuration &config);
private:
	bool found;
	std::uint8_t tap_count;
	std::uint8_t action_count;
	std::vector<Configuration> solutions;
};