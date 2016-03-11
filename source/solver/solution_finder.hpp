#include "board.hpp"
#include "configuration.hpp"
#include "solution_list.hpp"

class Solution_Finder {
public:
	Solution_Finder(int min_taps, int max_taps);

	long long int get_solutions_checked();
	Solution_List get_solution_list();

	void find(const Configuration& configuration);

private:
	void find_all(const Configuration& configuration);

private:
	int min_taps;
	int max_taps;
	long long int solutions_checked;
	Board_State state;
	Solution_List list;
};
