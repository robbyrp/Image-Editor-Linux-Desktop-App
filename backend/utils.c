// Functions in this module DO NOT CHECK if image != NULL. That is the responsibility of the caller function!
#include "image.h"


bool image_is_color(image_t *image)
{
    return image->color_matrix && !image->greyscale_matrix;
}

bool image_is_greyscale(image_t *image) 
{
    return !image->color_matrix && image->greyscale_matrix;
}

bool is_binary(image_t *image)
{
    if (strcmp(image->format, "P6") == 0 || strcmp(image->format, "P5") == 0) {
		return true;
	}
	return false;
}
