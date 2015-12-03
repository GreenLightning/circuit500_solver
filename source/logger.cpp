#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>

#include <boost/date_time.hpp>

#include "logger.hpp"

namespace pt = boost::posix_time;

Logger::Logger() {
	std::ostringstream log_name;
	log_name.imbue(std::locale(log_name.getloc(), new pt::time_facet("%Y-%m-%d_%H.%M.%S")));
	log_name << "data/logs/log_" << pt::microsec_clock::local_time() << ".txt";
	log_file = std::ofstream(log_name.str());
	log_file << "level; solutions; nanoseconds" << std::endl;
	count = 0;
	current_level = 0;
}

long long int & Logger::start_search(int level) {
	if (current_level != 0)
		throw std::runtime_error("search already started");
	current_level = level;
	solutions_checked = 0;
	start = std::chrono::high_resolution_clock::now();
	return solutions_checked;
}

void Logger::stop_search() {
	if (current_level == 0)
		throw std::runtime_error("search not started");
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
	log_file << std::setw(5) << current_level << ";" << std::setw(10) << solutions_checked << ";" << std::setw(12) << duration  << "\n";
	if (++count % 10 == 0) log_file.flush();
	current_level = 0;
}