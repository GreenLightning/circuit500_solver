#ifndef LOGGER
#define LOGGER

class Logger {
public:
	static Logger *create_file_logger();
	static Logger *create_fake_logger();

	virtual ~Logger() {}
	virtual long long int &start_search(int level) = 0;
	virtual void stop_search(int taps, int actions) = 0;
};

#endif