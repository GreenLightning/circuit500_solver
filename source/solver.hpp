#include "logger.hpp"
#include "rgb_image.hpp"
#include "solver/configuration.hpp"

class Solver {
public:
	static int tap_minimum();
	static int tap_maximum();

	Solver(Logger& logger, int max_taps);
	~Solver();

	void solve_level(int level_number);

private:
	Configuration* load_level(std::string filename, bool& error);
	void free_level(Configuration* level);

	std::string level_filename(int level_number);
	std::string solution_filename(int level_number);

public:
	bool unsolved;
	bool dry;

private:
	int max_taps;

private:
	Logger& logger;
	RGB_Image** reference_images;
};
