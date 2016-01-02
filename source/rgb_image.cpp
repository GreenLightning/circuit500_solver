#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "rgb_image.hpp"

RGB_Image::RGB_Image() :
	data(nullptr), width(0), height(0), stride(0), owns_data(false) {}

RGB_Image::RGB_Image(unsigned char * data, unsigned int width, unsigned int height, unsigned int stride, bool owns_data) :
	data(data), width(width), height(height), stride(stride), owns_data(owns_data) {}

RGB_Image::RGB_Image(RGB_Image &&image) noexcept :
	data(image.data), width(image.width), height(image.height), stride(image.stride), owns_data(image.owns_data) {
	image.data = nullptr;
	image.width = image.width = image.stride = 0;
	image.owns_data = false;
}

RGB_Image& RGB_Image::operator=(RGB_Image &&image) noexcept {
	std::swap(this->data, image.data);
	std::swap(this->width, image.width);
	std::swap(this->height, image.height);
	std::swap(this->stride, image.stride);
	std::swap(this->owns_data, image.owns_data);
	return *this;
}

RGB_Image::~RGB_Image() {
	if (owns_data) delete [] data;
}

std::string RGB_Image::error_text = "";

inline bool RGB_Image::is_error() {
	return data == nullptr;
}

std::string RGB_Image::get_error_text() {
	return error_text;
}

inline RGB_Image RGB_Image::create_error() {
	return RGB_Image();
}

inline void RGB_Image::report_error_void(bool &error, std::string error_text) {
	RGB_Image::error_text = error_text;
	error = true;
}

inline bool RGB_Image::report_error_bool(bool &error, std::string error_text) {
	report_error_void(error, error_text);
	return false;
}

inline RGB_Image RGB_Image::report_error_img(bool &error, std::string error_text) {
	report_error_void(error, error_text);
	return create_error();
}

RGB_Image RGB_Image::create(unsigned int width, unsigned int height, bool &error) {
	if (error) return create_error();
	unsigned char *data = new unsigned char[width * height * 3];
	if (!data) return report_error_img(error, "out of memory");
	return RGB_Image(data, width, height, width * 3, true);
}

RGB_Image RGB_Image::create(unsigned int width, unsigned int height, RGB_Image_Color color, bool &error) {
	RGB_Image image = create(width, height, error);
	if (error) return image;
	unsigned char r = (color >> 16) & 0xff, g = (color >> 8) & 0xff, b = color & 0xff;
	for (unsigned int i = 0; i < width * height; ++i) {
		image.data[3 * i + 0] = r;
		image.data[3 * i + 1] = g;
		image.data[3 * i + 2] = b;
	}
	return image;
}

RGB_Image RGB_Image::load(std::string filename, bool &error) {
	if (error) return create_error();
	int width, height, file_components;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &file_components, 3);
	if (!data) return report_error_img(error, std::string("failed to load image: ") + std::string(stbi_failure_reason()));
	return RGB_Image(data, width, height, width * 3, true);
}

RGB_Image RGB_Image::load(std::string filename, unsigned int expected_width, unsigned int expected_height, bool &error) {
	if (error) return create_error();
	RGB_Image image = load(filename, error);
	if (error) return image;
	if (image.width == expected_width && image.height == expected_height) return image;
	std::ostringstream error_text;
	error_text << "expected dimensions [" << expected_width << ", " << expected_height << "] ";
	error_text << "but found [" << image.width << ", " << image.height << "]";
	return report_error_img(error, error_text.str());
}

void RGB_Image::save(std::string filename, bool &error) {
	if (error) return;
	if (is_error()) return report_error_void(error, "image is invalid");
	bool success = stbi_write_png(filename.c_str(), width, height, 3, data, stride);
	if (!success) report_error_void(error, "failed to save image");
}

RGB_Image RGB_Image::create_copy(bool &error) {
	if (error) return create_error();
	if (is_error()) return report_error_img(error, "image is invalid");
	RGB_Image copy = create(width, height, error);
	copy_to(copy, error);
	return copy;
}

RGB_Image RGB_Image::create_copy(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error) {
	if (error) return create_error();
	if (is_error()) return report_error_img(error, "image is invalid");
	RGB_Image copy = create(width, height, error);
	create_view(x, y, width, height, error).copy_to(copy, error);
	return copy;
}

RGB_Image RGB_Image::create_view(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error) {
	if (error) return create_error();
	if (is_error()) return report_error_img(error, "image is invalid");
	return RGB_Image(getData(x, y), width, height, stride, false);
}

void RGB_Image::tint(RGB_Image_Color color, bool &error) {
	if (error) return;
	if (is_error()) return report_error_void(error, "image is invalid");
	unsigned char alpha_byte = color >> 24;
	float alpha = alpha_byte / 255.0f;
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned char *pixel = getData(x, y);
			for (unsigned int i = 0; i < 3; ++i) {
				unsigned char pixel_byte = pixel[i];
				unsigned char color_byte = color >> ((2 - i) * 8);
				float pixel_value = pixel_byte / 255.0f;
				float color_value = color_byte / 255.0f;
				float result_value = alpha * color_value + (1 - alpha) * pixel_value;
				unsigned char result_byte = result_value * 255;
				pixel[i] = result_byte;
			}
		}
	}
}

bool RGB_Image::equals(RGB_Image &other, bool &error) {
	if (error) return false;
	if (this->is_error()) return report_error_bool(error, "this image is invalid");
	if (other.is_error()) return report_error_bool(error, "other image is invalid");
	if (this->width != other.width || this->height != other.height) {
		std::ostringstream error_text;
		error_text << "different image dimensions (";
		error_text << "this: [" << this->width << ", " << this->height << "]; ";
		error_text << "other: [" << other.width << ", " << other.height << "])";
		return report_error_bool(error, error_text.str());
	}
	for (unsigned int y = 0; y < this->height; ++y)
		if (memcmp(this->getData(0, y), other.getData(0, y), this->width * 3) != 0)
			return false;
	return true;
}

void RGB_Image::copy_to(RGB_Image &destination, bool &error) {
	if (error) return;
	if (this->is_error()) return report_error_void(error, "source image is invalid");
	if (destination.is_error()) return report_error_void(error, "destination image is invalid");
	if (this->width != destination.width || this->height != destination.height) {
		std::ostringstream error_text;
		error_text << "different image dimensions (";
		error_text << "source: [" << this->width << ", " << this->height << "]; ";
		error_text << "destination: [" << destination.width << ", " << destination.height << "])";
		return report_error_void(error, error_text.str());
	}
	for (unsigned int y = 0; y < this->height; ++y)
		std::memcpy(destination.getData(0, y), this->getData(0, y), this->width * 3);
}