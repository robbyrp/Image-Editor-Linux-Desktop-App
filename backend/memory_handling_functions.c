// Functions that allocate the exact amount of memory on the heap
// Based on the type of image that was processed

#include "load_save.h"

void alloc_greyscale(image_t *image)
{
	#define UC unsigned char
	image->greyscale_matrix = (UC **)malloc((size_t)image->rows * sizeof(UC *));
	image->greyscale_memblock = (UC *)malloc((size_t)image->rows * (size_t)image->cols * sizeof(image->greyscale_memblock));

	if (!image->greyscale_matrix || !image->greyscale_memblock) {
		free_greyscale(image);
		fprintf(stderr, "Malloc() failed for greyscale array\n");
		return;
	}

	for (int i = 0; i < image->rows; i++) {
		image->greyscale_matrix[i] = image->greyscale_memblock + (size_t)i * image->cols;
	}
	#undef UC
}

void alloc_color(image_t *image)
{
	image->color_matrix = (pixel_t **)malloc((size_t)image->rows * sizeof(pixel_t *));
	image->color_memblock = (pixel_t *)malloc((size_t)image->rows * (size_t)image->cols * sizeof(image->color_memblock));
	if (!image->color_matrix || !image->color_memblock) {
		free_color(image);
		fprintf(stderr, "Malloc() failed for color matrix\n");
		return;
	}

	for (int i = 0; i < image->rows; i++) {
		image->color_matrix[i] = image->color_memblock + (size_t)i * image->cols;
	}
}

void free_greyscale(image_t *image)
{
	free(image->greyscale_memblock);
	free(image->greyscale_matrix);
	image->greyscale_matrix= NULL;
	image->greyscale_memblock = NULL;

}

void free_color(image_t *image)
{
	free(image->color_memblock);
	free(image->color_matrix);
	image->color_matrix = NULL;
	image->color_memblock = NULL;
}
