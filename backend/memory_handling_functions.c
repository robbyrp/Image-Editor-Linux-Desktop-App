// Functions that allocate the exact amount of memory on the heap
// Based on the type of image that was processed

#include "def.h"

void alloc_greyscale(image_t *image)
{
	#define UC unsigned char
	image->greyscale_matrix = (UC * *)malloc(image->rows * sizeof(UC *));
	if (!image->greyscale_matrix) {
		fprintf(stderr, "Malloc() failed for greyscale array\n");
		return;
	}
	for (int i = 0; i < image->rows; i++) {
		image->greyscale_matrix[i] = (UC *)malloc(image->cols * sizeof(UC));
		if (!image->greyscale_matrix[i]) {
			fprintf(stderr, "Malloc() failed for greyscale array\n");
			return;
		}
	}
	#undef UC
}

void alloc_color(image_t *image)
{
	image->color_matrix = (pixel_t **)malloc(image->rows * sizeof(pixel_t *));
	if (!image->color_matrix) {
		fprintf(stderr, "Malloc() failed for color matrix\n");
		return;
	}
	for (int i = 0; i < image->rows; i++) {
		image->color_matrix[i] =
		(pixel_t *)malloc(image->cols * sizeof(pixel_t));
		if (!image->color_matrix[i]) {
			fprintf(stderr, "Malloc() failed for color matrix\n");
			return;
		}
	}
}

void free_greyscale(image_t *image)
{
	if (image->greyscale_matrix) {
		for (int i = 0; i < image->rows; i++) {
			free(image->greyscale_matrix[i]);
		}
		free(image->greyscale_matrix);
		image->greyscale_matrix = NULL;
	}

}

void free_color(image_t *image)
{
	if (image->color_matrix) {
		for (int i = 0; i < image->rows; i++) {
			free(image->color_matrix[i]);
		}
		free(image->color_matrix);
		image->color_matrix = NULL;
	}
}

// Function that exits the program

void exit_program(image_t *image)
{
	if (!image->greyscale_matrix && !image->color_matrix) {
		printf("No image loaded\n");
	}
	if (image->color_matrix) {
		free_color(image);
	}
	if (image->greyscale_matrix) {
		free_greyscale(image);
	}
}
