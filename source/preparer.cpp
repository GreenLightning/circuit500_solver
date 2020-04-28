#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "rgb_image.hpp"

#include "preparer.hpp"

namespace fs = std::filesystem;

constexpr int digit_width = 8;
constexpr int digit_height = 9;

constexpr int raw_base_width = 160;
constexpr int raw_base_height = 284;

constexpr int level_number_x = 90;
constexpr int level_number_y = 25;

constexpr int area_x = 16;
constexpr int area_y = 47;
constexpr int area_width = 128;
constexpr int area_height = 192;

Preparer::Preparer()
	: load_error(false), buffer(create_buffer()), digits(load_digits()) {}

Preparer::~Preparer() {
	delete [] digits;
}

RGB_Image Preparer::create_buffer() {
	RGB_Image buffer = RGB_Image::create(raw_base_width, raw_base_height, load_error);
	if (load_error) std::cout << "error: " << RGB_Image::get_error_text() << std::endl;
	return buffer;
}

RGB_Image** Preparer::load_digits() {
	if (load_error) return nullptr;
	RGB_Image** digits = new RGB_Image*[10];
	if (digits == nullptr) {
		std::cout << "error: out of memory while allocating digit array" << std::endl;
		load_error = true;
		return nullptr;
	}
	for (int i = 0; i < 10; ++i) {
		std::ostringstream filename;
		filename << "data/reference/digit_" << i << ".png";
		digits[i] = new RGB_Image(RGB_Image::load(filename.str(), digit_width, digit_height, load_error));
	}
	return digits;
}

bool Preparer::is_initialized() {
	return !load_error;
}

int Preparer::prepare(fs::path file) {
	if (load_error) {
		std::cout << "error" << std::endl;
		return 0;
	}
	bool prepare_error = false;
	RGB_Image raw = RGB_Image::load(file.string(), prepare_error);
	if (prepare_error) {
		std::cout << "failed to load: " << RGB_Image::get_error_text() << ": skipped" << std::endl;
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
			unsigned char* buffer_pos = buffer.getData(x, y);
			unsigned char* raw_pos = raw.getData(x * scale_x, y * scale_y);
			for (int component = 0; component < 3; ++component) buffer_pos[component] = raw_pos[component];
		}
	}
	int level = find_level_number();
	if (level == 0) {
		std::cout << "failed to detect level number" << std::endl;
		return 0;
	}
	std::cout << "level " << std::setfill(' ') << std::setw(3) << level << ": ";
	std::ostringstream filename;
	filename << "data/levels/level_" << std::setfill('0') << std::setw(3) << level << ".png";
	RGB_Image area = buffer.create_view(area_x, area_y, area_width, area_height, prepare_error);
	area.save(filename.str(), prepare_error);
	std::cout << (prepare_error ? RGB_Image::get_error_text() : "success") << std::endl;
	return prepare_error ? 0 : level;
}

int Preparer::find_level_number() {
	bool image_error = false;
	int level = 0;
	for (int digit_offset = 0; ; ++digit_offset) {
		int digit_index = 0;
		for (; digit_index < 10; ++digit_index) {
			int buffer_digit_x = level_number_x + digit_offset * digit_width;
			int buffer_digit_y = level_number_y;
			RGB_Image buffer_digit = buffer.create_view(buffer_digit_x, buffer_digit_y, digit_width, digit_height, image_error);
			RGB_Image* test_digit = digits[digit_index];
			if (buffer_digit.equals(*test_digit, image_error)) break;
			if (image_error) { std::cout << RGB_Image::get_error_text() << ": "; return 0; }
		}
		if (digit_index == 10) return level;
		level = 10 * level + digit_index;
	}
}
