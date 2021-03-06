#pragma once

#include <string>

class Logger {
public:
	static Logger* create_file_logger(const std::string& header);
	static Logger* create_fake_logger(const std::string& header);

	virtual ~Logger() {}
	virtual long long int& start_search(int level) = 0;
	virtual void stop_search(int taps, int actions) = 0;
};
