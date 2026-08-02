#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <stdbool.h>
	#include <string.h> 

    #define ASCII_PIXEL_ESTIMATE 4

    typedef struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} pixel_t;

	typedef struct {
		char format[5];								// P2 P3 P5 P6
		int cols;
		int rows;
		int maxval; 								// 255
		unsigned char **greyscale_matrix;			// 0-255 value matrix
		unsigned char *greyscale_memblock;			// contiguous memory block for greyscale matrix
		pixel_t **color_matrix;						// rgb matrix
		pixel_t *color_memblock;					// contiguous memory block for color matrix
		bool loaded; 								// if the image is loaded in the gui
	} image_t;

	typedef struct {
		int x_start;
		int y_start;
		int x_end;
		int y_end;
		bool all;							// if the selection is the entire image
		bool changed;						// if the selection was changed by the user
		bool is_valid;						// if the selection is valid
	} selection_t;

    void free_greyscale(image_t *image);
	void free_color(image_t *image);

	void alloc_greyscale(image_t *image);
	void alloc_color(image_t *image);

	image_t* clone_image(image_t *dest, image_t *source);

	bool image_is_color(image_t *image);
	bool image_is_greyscale(image_t *image);
	bool is_binary(image_t *image);

#ifdef __cplusplus
}
#endif

#endif