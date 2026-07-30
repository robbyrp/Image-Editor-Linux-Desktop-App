// Functions that crop the image

#include "image_operations.h"

static void alloc_cropped_greyscale(image_t *cropped);
static void alloc_cropped_color(image_t *cropped);

int check_selection(image_t *image, selection_t *select)
{
	if (select->x_start > select->x_end) {
		int tmp = select->x_start;
		select->x_start = select->x_end;
		select->x_end = tmp;
	}
	if (select->y_start > select->y_end) {
		int tmp = select->y_start;
		select->y_start = select->y_end;
		select->y_end = tmp;
	}
	if (select->x_end > image->cols ||
		select->y_end > image->rows ||
		select->x_start < 0 || select->y_start < 0 ||
		select->x_start == select->x_end ||
		select->y_start == select->y_end) {
		return 1;
	}
	return 0;
}

void select_all(image_t *image, selection_t *select) 
{
		select->all = true;
		select->x_start = 0;
		select->y_start = 0;
		select->x_end = image->cols;
		select->y_end = image->rows;
		return;
}

void crop_region(image_t *image, selection_t *select)
{
	if (!image->color_matrix && !image->greyscale_matrix) {
		printf("No image loaded\n");
		return;
	}

	image_t cropped;
	//SHALLOW COPY OF IMAGE
	cropped = *image;
	cropped.color_matrix = NULL; cropped.greyscale_matrix = NULL;
	cropped.rows = select->y_end - select->y_start;
	cropped.cols = select->x_end - select->x_start;

	// LOADED GREYSCALE MATRIX
	if (!image->color_matrix && image->greyscale_matrix) {

		// MALLOC GREYSCALE CROPPED MATRIX
		alloc_cropped_greyscale(&cropped);

		// ASSIGN VALUES
		for (int i = 0; i < cropped.rows; i++) {
			memcpy(cropped.greyscale_matrix[i], &image->greyscale_matrix[select->y_start + i][select->x_start],
			(size_t)cropped.cols * sizeof(unsigned char));
		}

		// FREE IMAGE POINTER
		free_greyscale(image);
	}

	// LOADED COLOR MATRIX
	if (image->color_matrix && !image->greyscale_matrix) {

		// MALLOC COLOR CROPPED MATRIX
		alloc_cropped_color(&cropped);

		// ASSIGN VALUES
		for (int i = 0; i < cropped.rows; i++) {
			memcpy(cropped.color_matrix[i], &image->color_matrix[select->y_start + i][select->x_start],
			(size_t)cropped.cols * sizeof(pixel_t));
		}
		
		// FREE IMAGE POINTER
		free_color(image);
	}

	// IMAGE BECOMES CROPPED BY COPYING ALL FIELDS AT ONCE
	 *image = cropped;

	// SELECTION IS NOW THE ENTIRE NEW IMAGE
	select->x_start = 0;
	select->y_start = 0;
	select->x_end = cropped.cols;
	select->y_end = cropped.rows;
	select->all = true;
}

static void alloc_cropped_greyscale(image_t *cropped)
{
	#define UC unsigned char
	cropped->greyscale_matrix = (UC **)malloc((cropped->rows) * sizeof(UC *));
	if (!cropped->greyscale_matrix) {
		fprintf(stderr, "Malloc() failed for cropped greyscale\n");
		return;
	}
	for (int i = 0; i < cropped->rows; i++)	{
		cropped->greyscale_matrix[i] =
		(UC *)malloc((cropped->cols) * sizeof(UC));
		if (!cropped->greyscale_matrix[i]) {
			fprintf(stderr, "Malloc() failed for cropped greyscale\n");
			return;
		}
	}
	#undef UC
}

static void alloc_cropped_color(image_t *cropped)
{
	cropped->color_matrix =
	(pixel_t **)malloc(cropped->rows * sizeof(pixel_t *));
	if (!cropped->color_matrix) {
		fprintf(stderr, "Malloc() failed for cropped color\n");
		return;
	}
	for (int i = 0; i < cropped->rows; i++) {
		cropped->color_matrix[i] =
		(pixel_t *)malloc(cropped->cols * sizeof(pixel_t));
		if (!cropped->color_matrix[i]) {
			fprintf(stderr, "Malloc() failed for cropped color\n");
			return;
		}
	}
}
