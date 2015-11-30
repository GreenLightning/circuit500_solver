#include <vector>

#include <boost/filesystem.hpp>
#include <boost/icl/interval_set.hpp>

icl::interval_set<int> prepare_files(std::vector<boost::filesystem::path> &files);