#ifndef LOGGER
#define LOGGER

#include <fstream>
#include <chrono>

class Logger {
public:
	Logger();
	long long int &start_search(int level);
	void stop_search();
private:
	std::ofstream log_file;
	int count;
	int current_level;
	long long int solutions_checked;
	std::chrono::high_resolution_clock::time_point start;
};

#endif