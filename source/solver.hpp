#include "solver/configuration.hpp"
#include "solver/solution_list.hpp"
#include "solver/board.hpp"
#include "logger.hpp"
#include "rgb_image.hpp"

class Solver {
public:
	Solver(Logger &logger);
	~Solver();

	void solve_level(int level_number);

private:
	Configuration *load_level(std::string filename, bool &error);
	void free_level(Configuration *level);

	std::string level_filename(int level_number);
	std::string solution_filename(int level_number);

	Solution_List find_solution_list(Configuration level, long long int &solutions_checked);
	void find_solution_helper(Solution_List &list, Board_State &state, long long int &solutions_checked, const Configuration &solution);

public:
	bool unsolved;

private:
	Logger &logger;
	RGB_Image **reference_images;
};