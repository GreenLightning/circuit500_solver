#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/icl/interval_set.hpp>

#include "rgb_image.hpp"

namespace fs = boost::filesystem;
namespace icl = boost::icl;

const int digit_width = 8;
const int digit_height = 9;

const int raw_base_width = 160;
const int raw_base_height = 284;

const int level_number_x = 90;
const int level_number_y = 25;

const int area_x = 16;
const int area_y = 47;
const int area_width = 128;
const int area_height = 192;

RGB_Image create_buffer(bool &error);
RGB_Image **load_digits(bool &error);

void process_files(RGB_Image &buffer, RGB_Image **digits, std::vector<fs::path> &files, icl::interval_set<int> &processed);
int process_file(RGB_Image &buffer, RGB_Image **digits, fs::path file);
int find_level_number(RGB_Image **digits, RGB_Image &raw);

icl::interval_set<int> prepare_files(std::vector<fs::path> &files) {
	bool error = false;
	icl::interval_set<int> processed;
	RGB_Image buffer = create_buffer(error);
	RGB_Image **digits = load_digits(error);
	if (!error) process_files(buffer, digits, files, processed);
	delete [] digits;
	return processed;
}

RGB_Image create_buffer(bool &error) {
	RGB_Image buffer = rgb_image_create(raw_base_width, raw_base_height, error);
	if (error) std::cout << "Error: " << rgb_image_error_text() << "." << std::endl;
	return buffer;
}

RGB_Image **load_digits(bool &error) {
	if (error) return nullptr;
	RGB_Image **digits = new RGB_Image*[10];
	if (digits == nullptr) {
		std::cout << "Error: Out of memory while allocating digit array." << std::endl;
		error = true;
		return nullptr;
	}
	for (int i = 0; i < 10; ++i) {
		std::ostringstream filename;
		filename << "data/reference/digit_" << i << ".png";
		digits[i] = new RGB_Image(rgb_image_load(filename.str(), digit_width, digit_height, error));
	}
	return digits;
}

void process_files(RGB_Image &buffer, RGB_Image **digits, std::vector<fs::path> &files, icl::interval_set<int> &processed) {
	int count = 0, total = files.size();
	for (auto&& it : files) {
		int progress = 100 * (++count) / total;
		std::cout << std::setfill(' ') << std::setw(3) << progress << "%: " << it.generic_string() << ": ";
		int result = process_file(buffer, digits, it);
		if (result) processed.insert(result);
	}
}

int process_file(RGB_Image &buffer, RGB_Image **digits, fs::path file) {
	bool error = false;
	RGB_Image raw = rgb_image_load(file.string(), error);
	if (error) {
		std::cout << "failed to load: " << rgb_image_error_text() << ": skipped" << std::endl;
		return 0;
	}
	if (raw.getWidth() < raw_base_width || raw.getHeight() < raw_base_height) {
		std::cout << "image too small: ";
		std::cout << "expected at least [" << raw_base_width << ", " << raw_base_height << "]: ";
		std::cout << "actual [" << raw.getWidth() << ", " << raw.getHeight() << "]: skipped" << std::endl;
		return 0;
	}
	if (raw.getWidth() % raw_base_width != 0 || raw.getHeight() % raw_base_height != 0) {
		std::cout << "unexpected dimensions: ";
		std::cout << "expected clean multiple of [" << raw_base_width << ", " << raw_base_height << "]: ";
		std::cout << "actual [" << raw.getWidth() << ", " << raw.getHeight() << "]: skipped" << std::endl;
		return 0;
	}
	int scale_x = raw.getWidth() / raw_base_width;
	int scale_y = raw.getHeight() / raw_base_height;
	for (int y = 0; y < raw_base_height; ++y) {
		for (int x = 0; x < raw_base_width; ++x) {
			unsigned char *buffer_pos = buffer.getData(x, y);
			unsigned char *raw_pos = raw.getData(x * scale_x, y * scale_y);
			for (int component = 0; component < 3; ++component) buffer_pos[component] = raw_pos[component];
		}
	}
	int level = find_level_number(digits, buffer);
	if (level == 0) {
		std::cout << "failed to detect level number" << std::endl;
		return 0;
	}
	std::cout << "Level " << std::setfill('0') << std::setw(3) << level << ": ";
	std::ostringstream filename;
	filename << "data/levels/level_" << std::setfill('0') << std::setw(3) << level << ".png";
	RGB_Image area = rgb_image_create_view(buffer, area_x, area_y, area_width, area_height, error);
	rgb_image_save(filename.str(), area, error);
	std::cout << (error ? rgb_image_error_text() : "success") << std::endl;
	return error ? 0 : level;
}

int find_level_number(RGB_Image **digits, RGB_Image &raw) {
	bool error = false;
	int level = 0;
	int digit_offset = 0;
	while (true) {
		int digit_index = 0;
		for (; digit_index < 10; ++digit_index) {
			int raw_digit_x = level_number_x + digit_offset * digit_width;
			int raw_digit_y = level_number_y;
			RGB_Image raw_digit = rgb_image_create_view(raw, raw_digit_x, raw_digit_y, digit_width, digit_height, error);
			RGB_Image *test_digit = digits[digit_index];
			if (rgb_image_compare(raw_digit, *test_digit, error)) break;
			if (error) { std::cout << rgb_image_error_text() << ": "; return 0; }
		}
		if (digit_index == 10) return level;
		level = 10 * level + digit_index;
		++digit_offset;
	}
}