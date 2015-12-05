#ifndef LOGGER
#define LOGGER

#include <fstream>
#include <chrono>

class Logger {
public:
	Logger();
	~Logger();
	long long int &start_search(int level);
	void stop_search();
private:
	std::ofstream log_file;
	
	int level_count;
	long long int total_solutions;
	std::chrono::nanoseconds total_time;

	int current_level;
	long long int solutions_checked;
	std::chrono::high_resolution_clock::time_point start;
};

#endif