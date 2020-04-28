#pragma once

#include <cstdio>

constexpr size_t level_count = 500;
constexpr size_t level_begin = 1;
constexpr size_t level_end = level_count + 1;

class Level_Set {
public:
	void set_all();
	bool is_full();
	bool is_empty();

	// level is a 1-based index
	bool is_set(size_t level);
	void set(size_t level);
	void set_inclusive(size_t low, size_t high);

private:
	bool solve[level_count] = {};
};
