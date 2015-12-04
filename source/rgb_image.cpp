#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string>
#include <cstring>
#include <sstream>
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

static std::string rgb_image_error;

inline RGB_Image rgb_image_create_error() {
	return RGB_Image();
}

inline void rgb_image_error_void(bool &error, std::string error_text) {
	rgb_image_error = error_text;
	error = true;
}

inline bool rgb_image_error_bool(bool &error, std::string error_text) {
	rgb_image_error_void(error, error_text);
	return false;
}

inline RGB_Image rgb_image_error_img(bool &error, std::string error_text) {
	rgb_image_error_void(error, error_text);
	return rgb_image_create_error();
}

RGB_Image rgb_image_create(unsigned int width, unsigned int height, bool &error) {
	if (error) return rgb_image_create_error();
	unsigned char *data = new unsigned char[width * height * 3];
	if (!data) return rgb_image_error_img(error, "out of memory");
	return RGB_Image(data, width, height, width * 3, true);
}

RGB_Image rgb_image_create(unsigned int width, unsigned int height, RGB_Image_Color color, bool &error) {
	RGB_Image image = rgb_image_create(width, height, error);
	if (error) return image;
	unsigned char r = (color >> 16) & 0xff, g = (color >> 8) & 0xff, b = color & 0xff;
	for (unsigned int i = 0; i < width * height; ++i) {
		image.data[3 * i + 0] = r;
		image.data[3 * i + 1] = g;
		image.data[3 * i + 2] = b;
	}
	return image;
}

RGB_Image rgb_image_create_copy(RGB_Image &image, bool &error) {
	if (error) return rgb_image_create_error();
	if (rgb_image_is_error(image)) return rgb_image_error_img(error, "image is invalid");
	RGB_Image copy = rgb_image_create(image.width, image.height, error);
	rgb_image_copy_from_to(image, copy, error);
	return copy;
}

RGB_Image rgb_image_create_copy(RGB_Image &image, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error) {
	if (error) return rgb_image_create_error();
	if (rgb_image_is_error(image)) return rgb_image_error_img(error, "image is invalid");
	RGB_Image copy = rgb_image_create(width, height, error);
	rgb_image_copy_from_to(rgb_image_create_view(image, x, y, width, height, error), copy, error);
	return copy;
}

RGB_Image rgb_image_create_view(RGB_Image &image, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error) {
	if (error) return rgb_image_create_error();
	if (rgb_image_is_error(image)) return rgb_image_error_img(error, "image is invalid");
	return RGB_Image(image.getData(x, y), width, height, image.stride, false);
}

RGB_Image rgb_image_load(std::string filename, bool &error) {
	if (error) return rgb_image_create_error();
	int width, height, file_components;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &file_components, 3);
	if (!data) return rgb_image_error_img(error, std::string("failed to load image: ") + std::string(stbi_failure_reason()));
	return RGB_Image(data, width, height, width * 3, true);
}

RGB_Image rgb_image_load(std::string filename, unsigned int expected_width, unsigned int expected_height, bool &error) {
	if (error) return rgb_image_create_error();
	RGB_Image image = rgb_image_load(filename, error);
	if (error) return image;
	if (image.width == expected_width && image.height == expected_height) return image;
	std::ostringstream error_text;
	error_text << "expected dimensions [" << expected_width << ", " << expected_height << "] ";
	error_text << "but found [" << image.width << ", " << image.height << "]";
	return rgb_image_error_img(error, error_text.str());
}

void rgb_image_save(std::string filename, RGB_Image &image, bool &error) {
	if (error) return;
	if (rgb_image_is_error(image)) return rgb_image_error_void(error, "image is invalid");
	bool success = stbi_write_png(filename.c_str(), image.width, image.height, 3, image.data, image.stride);
	if (!success) rgb_image_error_void(error, "failed to save image");
}

void rgb_image_tint(RGB_Image &image, RGB_Image_Color color, bool &error) {
	if (error) return;
	if (rgb_image_is_error(image)) return rgb_image_error_void(error, "image is invalid");
	unsigned char alpha_byte = color >> 24;
	float alpha = alpha_byte / 255.0f;
	for (unsigned int y = 0; y < image.height; ++y) {
		for (unsigned int x = 0; x < image.width; ++x) {
			unsigned char *pixel = image.getData(x, y);
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

bool rgb_image_compare(RGB_Image &one, RGB_Image &two, bool &error) {
	if (error) return false;
	if (rgb_image_is_error(one)) return rgb_image_error_bool(error, "first image is invalid");
	if (rgb_image_is_error(two)) return rgb_image_error_bool(error, "second image is invalid");
	if (one.width != two.width || one.height != two.height) {
		std::ostringstream error_text;
		error_text << "different image dimensions (";
		error_text << "first: [" << one.width << ", " << one.height << "]; ";
		error_text << "second: [" << two.width << ", " << two.height << "])";
		return rgb_image_error_bool(error, error_text.str());
	}
	for (unsigned int y = 0; y < one.height; ++y)
		if (memcmp(one.getData(0, y), two.getData(0, y), one.width * 3) != 0)
			return false;
	return true;
}

void rgb_image_copy_from_to(RGB_Image &source, RGB_Image &destination, bool &error) {
	if (error) return;
	if (rgb_image_is_error(source)) return rgb_image_error_void(error, "source image is invalid");
	if (rgb_image_is_error(destination)) return rgb_image_error_void(error, "destination image is invalid");
	if (source.width != destination.width || source.height != destination.height) {
		std::ostringstream error_text;
		error_text << "different image dimensions (";
		error_text << "source: [" << source.width << ", " << source.height << "]; ";
		error_text << "destination: [" << destination.width << ", " << destination.height << "])";
		return rgb_image_error_void(error, error_text.str());
	}
	for (unsigned int y = 0; y < source.height; ++y)
		std::memcpy(destination.getData(0, y), source.getData(0, y), source.width * 3);
}

inline bool rgb_image_is_error(RGB_Image &image) {
	return image.data == nullptr;
}

std::string rgb_image_error_text() {
	return rgb_image_error;
}