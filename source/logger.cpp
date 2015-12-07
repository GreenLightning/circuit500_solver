#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>

#include <boost/date_time.hpp>

#include "logger.hpp"

namespace pt = boost::posix_time;

class File_Logger : public Logger {
public:
	File_Logger();
	virtual ~File_Logger();
	virtual long long int &start_search(int level);
	virtual void stop_search();
private:
	std::ofstream log_file;
	
	int level_count;
	long long int total_solutions;
	std::chrono::nanoseconds total_time;

	int current_level;
	long long int solutions_checked;
	std::chrono::high_resolution_clock::time_point start;
};

File_Logger::File_Logger() : level_count(0), total_solutions(0), total_time(std::chrono::nanoseconds(0)) {
	std::ostringstream log_name;
	log_name.imbue(std::locale(log_name.getloc(), new pt::time_facet("%Y-%m-%d_%H.%M.%S")));
	log_name << "data/logs/log_" << pt::microsec_clock::local_time() << ".txt";
	this->log_file = std::ofstream(log_name.str());
	this->log_file << "level; solutions; nanoseconds" << std::endl;
	this->current_level = 0;
}

File_Logger::~File_Logger() {
	log_file << "---------- totals -----------" << std::endl;
	log_file << std::setw(5) << level_count << ";";
	log_file << std::setw(10) << total_solutions << ";";
	log_file << std::setw(12) << total_time.count()  << "\n";
}

long long int & File_Logger::start_search(int level) {
	current_level = level;
	solutions_checked = 0;
	start = std::chrono::high_resolution_clock::now();

	return solutions_checked;
}

void File_Logger::stop_search() {
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

class Fake_Logger : public Logger {
public:

	virtual long long int &start_search(int level) {
		return dummy = 0;
	}

	virtual void stop_search() {}

private:
	long long int dummy;
};

Logger * Logger::create_file_logger() {
	return new File_Logger();
}

Logger * Logger::create_fake_logger() {
	return new Fake_Logger();
}