#include <boost/filesystem.hpp>

#include "rgb_image.hpp"

class Preparer {

public:
	Preparer();
	~Preparer();

private:
	RGB_Image create_buffer();
	RGB_Image** load_digits();

public:
	bool is_initialized();
	int prepare(boost::filesystem::path file);

private:
	int find_level_number();

private:
	bool load_error;
	RGB_Image buffer;
	RGB_Image** digits;
};
