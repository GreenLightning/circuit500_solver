#ifndef RGB_IMAGE
#define RGB_IMAGE

#include <string>

// 8-bit ARGB color.
typedef int32_t RGB_Image_Color;

// Contains a pointer to RGB image data and stores the width and height of the image.
// Deletes the pixel data it points to when destructed.
// No bounds checking is performed on pixel coordinates.

// Notes on class usage:
// Instances should be obtained using one of the static create() or load() functions.
// Since this class manages the (possibly large) chunk of memory that holds the image data,
// the normal copy constructor and assignment operator have been hidden with their move-equivalents,
// so that you explicitly have to use one of the copy() functions or the view() function.

// Notes on error handling:
// (1) error flag
// Critical functions all use an error flag.
// If this flag is already set, the functions act as no-ops.
// If the flag has been set by one of these functions,
// a short error description can be retrieved using
// the static get_error_text() function.
// (2) invalid instances
// Instances can be either valid or invalid.
// The state of an instance can be checked using the is_error() function.
// The image data and size of an invalid instance must not be accessed.
// If a critical function is called on an invalid instance, the error flag is set.
// Critical functions return invalid instances if the error flag is set.
class RGB_Image {

private:
	// Creates an invalid image.
	RGB_Image();
	// Creates a normal image.
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

private:
	static std::string error_text;
	static RGB_Image create_error();
	static void report_error_void(bool &error, std::string error_text);
	static bool report_error_bool(bool &error, std::string error_text);
	static RGB_Image report_error_img(bool &error, std::string error_text);

public:
	// Returns true if this image is invalid.
	bool is_error();
	// Returns a short error message for the last error that occured or the empty string if no error has occured yet.
	static std::string get_error_text();
	
	// Returns an image of the specified size. The contents of the image are unspecified.
	static RGB_Image create(unsigned int width, unsigned int height, bool &error);
	// Returns an image of the specified size, filled with the given color. The alpha channel of the color is ignored.
	static RGB_Image create(unsigned int width, unsigned int height, RGB_Image_Color color, bool &error);

	// IO functions currently only support PNG images.

	// Loads an image from the specified file.
	static RGB_Image load(std::string filename, bool &error);
	// Loads an image from the specified file and checks its size.
	// If the image does not have the expected dimensions, the error flag is set and an invalid image is returned.
	static RGB_Image load(std::string filename, unsigned int expected_width, unsigned int expected_height, bool &error);

	// Saves this image to the specified file.
	void save(std::string filename, bool &error);

	// Returns a copy of this image that can be used even after this image is destructed.
	RGB_Image create_copy(bool &error);
	// Returns a copy of the specified subsection of this image that can be used even after this image is destructed.
	RGB_Image create_copy(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error);
	// Returns a view of the specified subsection of this image.
	// Changes to the view are forwarded to this image and vice versa.
	// After this image is destructed, the data pointer of the view becomes obsolete and must not be used.
	// The application is responsible to ensure that the views of an image are not used after its destruction.
	RGB_Image create_view(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool &error);

	// Tints the whole image in the specified color using the formula ci = a * cc + (1 - a) * ci where
	// a = alpha of color, ci = a component from a pixel from the image, cc = the corresponding component of the color. 
	void tint(RGB_Image_Color color, bool &error);

	// Functions that expect another image set the error flag if the other image has different dimensions than this image.

	// Returns true if this image is equal to the other image.
	// Returns false if the images are not equal or
	// error is true, one of the images is invalid or the images have different dimensions.
	bool equals(RGB_Image &other, bool &error);
	// Copies all pixels from this image to the destination image.
	void copy_to(RGB_Image &destination, bool &error);
};

#endif