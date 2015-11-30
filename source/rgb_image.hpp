#ifndef RGB_IMAGE
#define RGB_IMAGE

#include <string>

// 8-bit ARGB color.
typedef int32_t RGB_Image_Color;

// Contains a pointer to RGB image data and stores the width and height of the image.
// Deletes the pixel data it points to when destructed.
// No bounds checking is performed on pixel coordinates.
class RGB_Image {

private:
	RGB_Image();
	RGB_Image(unsigned char *data, unsigned int width, unsigned int height, unsigned int stride, bool owns_data);

public:
	RGB_Image(RGB_Image &&image) noexcept;
	RGB_Image& operator=(RGB_Image &&image) noexcept;
	~RGB_Image();

public:
	unsigned char* getData() { return data; }
	const unsigned char* getData() const { return data; }
	unsigned char* getData(unsigned int x, unsigned int y) { return data + y * stride + x * 3; }
	const unsigned char* getData(unsigned int x, unsigned int y) const { return data + y * stride + x * 3; }
	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }

private:
	unsigned char *data;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	bool owns_data;

public:
	// Returns an invalid image that can be returned from a function in case of an error.
	friend RGB_Image rgb_image_create_error();

	// Functions with an error flag are no-ops if it is set.
	
	// Returns an image of the specified size. The contents of the image are unspecified.
	friend RGB_Image rgb_image_create(unsigned int width, unsigned int height, bool &error);
	// Returns an image of the specified size, filled with the given color. The alpha channel of the color is ignored.
	friend RGB_Image rgb_image_create(unsigned int width, unsigned int height, RGB_Image_Color color, bool &error);

	// Functions that expect an image will set the error flag and return an invalid image if the passed image is invalid.

	// Returns a copy of the given image that can be used even after the original image is destroyed.
	friend RGB_Image rgb_image_create_copy(RGB_Image &image, bool &error);
	// Returns a copy of the specified subsection of the given image that can be used even after the original image is destroyed.
	friend RGB_Image rgb_image_create_copy(RGB_Image &image, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error);
	// Returns a view of the specified subsection of the given image.
	// Changes to the view are forwarded to the original image and vice versa.
	// After the original image is destroyed, the data pointer of the view is invalid and must not be used.
	friend RGB_Image rgb_image_create_view(RGB_Image &image, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error);

	// IO functions currently only support PNG images.

	// Loads an image from the specified file.
	friend RGB_Image rgb_image_load(std::string filename, bool &error);
	// Loads an image from the specified file and checks its size.
	// If the image does not have the expected dimensions, the error flag is set and an invalid image is returned.
	friend RGB_Image rgb_image_load(std::string filename, unsigned int expected_width, unsigned int expected_height, bool &error);

	// Saves the image to the specified file.
	friend void rgb_image_save(std::string filename, RGB_Image &image, bool &error);

	// Tints the whole image in the specified color using the formula ci = a * cc + (1 - a) * ci where
	// a = alpha of color, ci = a component from a pixel from the image, cc = the corresponding component of the color. 
	friend void rgb_image_tint(RGB_Image &image, RGB_Image_Color color, bool &error);

	// Functions that expect two images will set the error flag if the two images have different dimensions.

	// Returns true if the two images are identical.
	// Returns false if error is true, one of the images is invalid or the images have differnet dimensions.
	friend bool rgb_image_compare(RGB_Image &one, RGB_Image &two, bool &error);
	// Copies all pixels from the source image to the destination image.
	friend void rgb_image_copy_from_to(RGB_Image &source, RGB_Image &destination, bool &error);

	// Returns true if the image is invalid.
	friend bool rgb_image_is_error(RGB_Image &image);
	// Returns a short error message for the last error that occured or the empty string if no error has occured yet.
	friend std::string rgb_image_error_text();
};

#endif