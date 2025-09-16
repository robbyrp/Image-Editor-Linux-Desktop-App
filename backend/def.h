// PANA ROBERT IONUT

#ifndef DEF_H
#define DEF_H

#ifdef __cplusplus
extern "C" {
	#endif
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include <stdbool.h>
	#include <string.h>

	typedef struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} pixel_t;

	typedef struct {
		char format[5];						// P2 P3 P5 P6
		int cols;
		int rows;
		int maxval; 						// 255
		unsigned char **greyscale_matrix;	// 0-255 value matrix
		pixel_t **color_matrix;				// rgb matrix
		bool loaded; 						// if the image is loaded in the gui
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

	// In order not to call function with 7 parameters, I created this struct
	typedef struct {
		image_t *image;						
		selection_t *selected_region;
		bool *conversion_needed;
		unsigned char **display_buffer;
		char *input_file_path;
		char *output_file_path;
	} image_editor_t;
	void load_cli(image_t *image, selection_t *select);
	void load_gui(image_t *image, selection_t *select, const char *filename);


	void load_ascii(image_t *image, const char *filename);
	void load_binary(image_t *image, const char *filename);

	void free_greyscale(image_t *image);
	void free_color(image_t *image);

	void alloc_greyscale(image_t *image);
	void alloc_color(image_t *image);

	void save_cli(image_t *image);
	void save_ascii_gui(image_t *image, const char *new_filename);
	void save_binary_gui(image_t *image, const char *new_filename);
	bool is_binary(image_t *image);


	void histogram(image_t *image);
	void histogram_greyscale(image_t *image, int x, int bin);

	void equalize(image_t *image);
	unsigned char clamp(double result);
	void equalize_greyscale(image_t *image);

	void select_region(image_t *image, selection_t *select);
	int check_selection(image_t *image, selection_t *select);
	void select_all(image_t *image, selection_t *select);
	bool has_letters(char *aux);

	void crop_region(image_t *image, selection_t *select);
	void alloc_cropped_greyscale(image_t *cropped);
	void alloc_cropped_color(image_t *cropped);

	void rotate_region(image_t *image, selection_t *select);
	void rotate_square_greyscale_90deg(image_t *image, selection_t *select);
	void rotate_square_color_90deg(image_t *image, selection_t *select);
	void rotate_square(image_t *image, selection_t *select, int degrees);
	void rotate_all_90deg(image_t *image, selection_t *select);
	void rotate_all(image_t *image, selection_t *select, int degrees);

	void apply(image_t *image, selection_t *select);
	void apply_sharpen(image_t *image, selection_t *select);
	void apply_edge(image_t *image, selection_t *select);
	void apply_blur(image_t *image, selection_t *select);
	void apply_gaussian_blur(image_t *image, selection_t *select);
	pixel_t apply_kernel(image_t *image,
						int ker[3][3], int i, int j, int apply_type);
	unsigned char clamp_apply(int result);

	void exit_program(image_t *image);


	#ifdef __cplusplus
}
#endif

#endif
