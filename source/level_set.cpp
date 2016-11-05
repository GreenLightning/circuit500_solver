#include "level_set.hpp"

void Level_Set::set_all() {
	for (size_t i = 0; i < level_count; ++i) solve[i] = true;
}

bool Level_Set::is_full() {
	for (size_t i = 0; i < level_count; ++i) if (!solve[i]) return false;
	return true;
}

bool Level_Set::is_empty() {
	for (size_t i = 0; i < level_count; ++i) if (solve[i]) return false;
	return true;
}

bool Level_Set::is_set(size_t level) {
	return solve[level - 1];
}

void Level_Set::set(size_t level) {
	solve[level - 1] = true;
}

void Level_Set::set_inclusive(size_t low, size_t high) {
	for (size_t level = low; level <= high; ++level) solve[level - 1] = true;
}
