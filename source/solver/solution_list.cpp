#include <cstdint>
#include <vector>

#include "configuration.hpp"

#include "solution_list.hpp"

inline void Solution_List::push_first(const Configuration &config) {
	tap_count = config.tap_count;
	action_count = config.action_count;
	solutions.push_back(config);
}

inline bool Solution_List::contains(const Configuration &config) {
	for (auto &&solution : solutions)
		if (boards_are_equal(config.board, solution.board))
			return true;
	return false;
}

bool Solution_List::should_skip(const Configuration &config) {
	return found && config.action_count > action_count;
}

void Solution_List::append(const Configuration &config) {
	if (!found) {
		found = true;
		push_first(config);
	} else if (config.tap_count <= tap_count && config.action_count <= action_count) {
		if (config.tap_count < tap_count || config.action_count < action_count) {
			solutions.clear();
			push_first(config);
		} else if (!contains(config)) {
			solutions.push_back(config);
		}
	}
}