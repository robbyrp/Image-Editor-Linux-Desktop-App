// PANA ROBERT IONUT

#ifndef DEF_H
#define DEF_H

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
	char format[5];
	int cols;
	int rows;
	int maxval;
	unsigned char **greyscale_matrix;
	pixel_t **color_matrix;
} image_t;

typedef struct {
	int x_start;
	int y_start;
	int x_end;
	int y_end;
	bool all;
} selection_t;

void load(image_t *image, selection_t *select);
void load_ascii(image_t *image, char *filename);
void load_binary(image_t *image, char *filename);

void free_greyscale(image_t *image);
void free_color(image_t *image);

void alloc_greyscale(image_t *image);
void alloc_color(image_t *image);

void save(image_t *image);
void save_ascii(image_t *image, char *new_filename);
void save_binary(image_t *image, char *new_filename);

void histogram(image_t *image);
void histogram_greyscale(image_t *image, int x, int bin);

void equalize(image_t *image);
unsigned char clamp(double result);
void equalize_greyscale(image_t *image);

void select_region(image_t *image, selection_t *select);
int check_selection(image_t *image, selection_t *select);
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
#endif
