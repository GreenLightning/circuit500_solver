#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>

#include <boost/date_time.hpp>

#include "logger.hpp"

namespace pt = boost::posix_time;

Logger::Logger() : level_count(0), total_solutions(0), total_time(std::chrono::nanoseconds(0)) {
	std::ostringstream log_name;
	log_name.imbue(std::locale(log_name.getloc(), new pt::time_facet("%Y-%m-%d_%H.%M.%S")));
	log_name << "data/logs/log_" << pt::microsec_clock::local_time() << ".txt";
	this->log_file = std::ofstream(log_name.str());
	this->log_file << "level; solutions; nanoseconds" << std::endl;
	this->current_level = 0;
}

Logger::~Logger() {
	log_file << "---------- totals -----------" << std::endl;
	log_file << std::setw(5) << level_count << ";";
	log_file << std::setw(10) << total_solutions << ";";
	log_file << std::setw(12) << total_time.count()  << "\n";
}

long long int & Logger::start_search(int level) {
	current_level = level;
	solutions_checked = 0;
	start = std::chrono::high_resolution_clock::now();

	return solutions_checked;
}

void Logger::stop_search() {
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

	level_count += 1;
	total_solutions += solutions_checked;
	total_time += duration;
	
	log_file << std::setw(5) << current_level << ";";
	log_file << std::setw(10) << solutions_checked << ";";
	log_file << std::setw(12) << duration.count()  << "\n";
	if (level_count % 10 == 0) log_file.flush();

	current_level = 0;
}