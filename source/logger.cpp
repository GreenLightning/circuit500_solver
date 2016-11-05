#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

#include "logger.hpp"

class File_Logger : public Logger {
public:
	File_Logger(const std::string& header);
	virtual ~File_Logger();
	virtual long long int& start_search(int level);
	virtual void stop_search(int taps, int actions);
private:
	std::ofstream log_file;
	
	int level_count;
	long long int total_solutions;
	std::chrono::nanoseconds total_time;
	int solved_level_count;

	int current_level;
	long long int solutions_checked;
	std::chrono::high_resolution_clock::time_point start;
};

struct separators : std::numpunct<char> {
   char do_thousands_sep() const { return ' '; }
   std::string do_grouping() const { return "\3"; }
};

File_Logger::File_Logger(const std::string& header)
	: level_count(0), total_solutions(0), total_time(std::chrono::nanoseconds(0)), solved_level_count(0) {

	char log_name[256];
	{ // format log name with local time
		std::time_t t = std::time(nullptr);
		if (!std::strftime(log_name, sizeof(log_name), "data/logs/log_%Y-%m-%d_%H.%M.%S.txt", std::localtime(&t))) {
			std::snprintf(log_name, sizeof(log_name), "data/logs/log.txt");
		}
	}

	log_file = std::ofstream(log_name);
	log_file.imbue(std::locale(log_file.getloc(), new separators));
	log_file << header << std::endl;
	log_file << "level;          solutions;        nanoseconds; taps; actions" << std::endl;
	current_level = 0;
}

File_Logger::~File_Logger() {
	double solved_percentage = (100.0 * solved_level_count) / level_count;

	log_file << "-------------------------- totals --------------------------\n";
	log_file << std::setw(5) << level_count << ";";
	log_file << std::setw(19) << total_solutions << ";";
	log_file << std::setw(19) << total_time.count()  << ";";
	log_file << std::setw(5) << solved_level_count << ";";
	log_file << std::setw(7) << std::fixed << std::setprecision(2) << solved_percentage << "%";
}

long long int& File_Logger::start_search(int level) {
	current_level = level;
	solutions_checked = 0;
	start = std::chrono::high_resolution_clock::now();
	return solutions_checked;
}

void File_Logger::stop_search(int taps, int actions) {
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

	bool solved = (taps >= 0) && (actions >= 0);

	level_count += 1;
	total_solutions += solutions_checked;
	total_time += duration;
	if (solved) solved_level_count += 1;

	log_file << std::setw(5) << current_level << ";";
	log_file << std::setw(19) << solutions_checked << ";";
	log_file << std::setw(19) << duration.count() << ";";
	if (solved) {
		log_file << std::setw(5) << taps << ";";
		log_file << std::setw(8) << actions << "\n";
	} else {
		log_file << std::setw(5) << "---" << ";";
		log_file << std::setw(8) << "---" << "\n";
	}

	if (level_count % 10 == 0) log_file.flush();
	current_level = 0;
}

class Fake_Logger : public Logger {
public:

	virtual long long int& start_search(int level) {
		return dummy = 0;
	}

	virtual void stop_search(int taps, int  actions) {}

private:
	long long int dummy;
};

Logger* Logger::create_file_logger(const std::string& header) {
	return new File_Logger(header);
}

Logger* Logger::create_fake_logger(const std::string& header) {
	return new Fake_Logger();
}
