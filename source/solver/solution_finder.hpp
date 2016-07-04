#include "board.hpp"
#include "configuration.hpp"
#include "solution_list.hpp"

class Solution_Finder {
public:
	Solution_Finder(int max_taps);

	long long int get_solutions_checked();
	Solution_List get_solution_list();

	void find(const Configuration& configuration, Board_Position start = Board_Position());

private:
	int max_taps;
	long long int solutions_checked;
	Board_State state;
	Solution_List list;
};
