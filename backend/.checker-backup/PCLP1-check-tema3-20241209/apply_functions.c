// Functions that apply different filters

#include "def.h"

void apply(image_t *image, selection_t *select)
{
	if (!image->color_matrix && !image->greyscale_matrix) {
		printf("No image loaded\n");
		return;
	}
	char *apply_type = strtok(NULL, "\n ");
	if (!apply_type) {
		printf("Invalid command\n");
		return;
	}

	// CHECK FOR VALID PARAMETERS
	if (image->greyscale_matrix) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (strcmp(apply_type, "SHARPEN") == 0) {
		apply_sharpen(image, select);
	} else if (strcmp(apply_type, "EDGE") == 0) {
		apply_edge(image, select);
	} else if (strcmp(apply_type, "BLUR") == 0) {
		apply_blur(image, select);
	} else if (strcmp(apply_type, "GAUSSIAN_BLUR") == 0) {
		apply_gaussian_blur(image, select);
	} else {
		printf("APPLY parameter invalid\n");
		return;
	}

}

void apply_sharpen(image_t *image, selection_t *select)
{
	int type = 2;
	int ker[3][3] = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};
	image_t new_image;
	//SHALLOW COPY OF IMAGE
	new_image = *image;
	new_image.color_matrix = NULL; new_image.greyscale_matrix = NULL;
	new_image.cols = image->cols;
	new_image.rows = image->rows;
	alloc_color(&new_image);

	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			new_image.color_matrix[i][j] = image->color_matrix[i][j];
		}
	}
	// APPLY
	for (int i = select->y_start; i < select->y_end; i++) {
		for (int j = select->x_start; j < select->x_end; j++) {
			new_image.color_matrix[i][j] =
			apply_kernel(image, ker, i, j, type);
		}
	}
	// FREE IMAGE POINTER
	free_color(image);
	// IMAGE BECOMES NEW IMAGE BY COPYING ALL FIELDS AT ONCE
	*image = new_image;
	printf("APPLY SHARPEN done\n");
}

void apply_edge(image_t *image, selection_t *select)
{
	int type = 1;
	int ker[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}
	};
	image_t new_image;
	//SHALLOW COPY OF IMAGE
	new_image = *image;
	new_image.color_matrix = NULL; new_image.greyscale_matrix = NULL;
	new_image.cols = image->cols;
	new_image.rows = image->rows;
	alloc_color(&new_image);
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			new_image.color_matrix[i][j] = image->color_matrix[i][j];
		}
	}
	// APPLY
	for (int i = select->y_start; i < select->y_end; i++) {
		for (int j = select->x_start; j < select->x_end; j++) {
			new_image.color_matrix[i][j] =
			apply_kernel(image, ker, i, j, type);
		}
	}
	// FREE IMAGE POINTER
	free_color(image);
	// IMAGE BECOMES NEW IMAGE BY COPYING ALL FIELDS AT ONCE
	*image = new_image;

	printf("APPLY EDGE done\n");
}

void apply_blur(image_t *image, selection_t *select)
{
	int type = 3;
	int ker[3][3] = {
		{1, 1, 1},
		{1, 1, 1,},
		{1, 1, 1}
	};
	image_t new_image;
	//SHALLOW COPY OF IMAGE
	new_image = *image;
	new_image.color_matrix = NULL; new_image.greyscale_matrix = NULL;
	new_image.cols = image->cols;
	new_image.rows = image->rows;
	alloc_color(&new_image);
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			new_image.color_matrix[i][j] = image->color_matrix[i][j];
		}
	}

	// APPLY
	for (int i = select->y_start; i < select->y_end; i++) {
		for (int j = select->x_start; j < select->x_end; j++) {
			new_image.color_matrix[i][j] =
			apply_kernel(image, ker, i, j, type);
		}
	}
	// FREE IMAGE POINTER
	free_color(image);
	// IMAGE BECOMES NEW IMAGE BY COPYING ALL FIELDS AT ONCE
	*image = new_image;
	if (!image->color_matrix) {
		printf("Failed to allocate memory for new image\n");
		return;
	}
	printf("APPLY BLUR done\n");
}

void apply_gaussian_blur(image_t *image, selection_t *select)
{
	int type = 4;
	int ker[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};
	image_t new_image;
	//SHALLOW COPY OF IMAGE
	new_image = *image;
	new_image.color_matrix = NULL; new_image.greyscale_matrix = NULL;
	new_image.cols = image->cols;
	new_image.rows = image->rows;
	alloc_color(&new_image);
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			new_image.color_matrix[i][j] = image->color_matrix[i][j];
		}
	}
	// APPLY
	for (int i = select->y_start; i < select->y_end; i++) {
		for (int j = select->x_start; j < select->x_end; j++) {
			new_image.color_matrix[i][j] =
			apply_kernel(image, ker, i, j, type);
		}
	}
	// FREE IMAGE POINTER
	free_color(image);
	// IMAGE BECOMES NEW IMAGE BY COPYING ALL FIELDS AT ONCE
	*image = new_image;
	if (!image->color_matrix) {
		printf("Failed to allocate memory for new image\n");
		return;
	}
	printf("APPLY GAUSSIAN_BLUR done\n");
}

pixel_t apply_kernel(image_t *image,
					 int ker[3][3], int i, int j, int apply_type)
{
	pixel_t color;
	if (i == 0 || j == 0 ||
		i == image->rows - 1 || j == image->cols - 1) {
		color = image->color_matrix[i][j];
	} else {
		int red = image->color_matrix[i - 1][j - 1].r * ker[0][0] +
		image->color_matrix[i - 1][j].r * ker[0][1] +
		image->color_matrix[i - 1][j + 1].r * ker[0][2] +
		image->color_matrix[i][j - 1].r * ker[1][0] +
		image->color_matrix[i][j].r * ker[1][1] +
		image->color_matrix[i][j + 1].r * ker[1][2] +
		image->color_matrix[i + 1][j - 1].r * ker[2][0] +
		image->color_matrix[i + 1][j].r * ker[2][1] +
		image->color_matrix[i + 1][j + 1].r * ker[2][2];

		int green = image->color_matrix[i - 1][j - 1].g * ker[0][0] +
		image->color_matrix[i - 1][j].g * ker[0][1] +
		image->color_matrix[i - 1][j + 1].g * ker[0][2] +
		image->color_matrix[i][j - 1].g * ker[1][0] +
		image->color_matrix[i][j].g * ker[1][1] +
		image->color_matrix[i][j + 1].g * ker[1][2] +
		image->color_matrix[i + 1][j - 1].g * ker[2][0] +
		image->color_matrix[i + 1][j].g * ker[2][1] +
		image->color_matrix[i + 1][j + 1].g * ker[2][2];

		int blue = image->color_matrix[i - 1][j - 1].b * ker[0][0] +
		image->color_matrix[i - 1][j].b * ker[0][1] +
		image->color_matrix[i - 1][j + 1].b * ker[0][2] +
		image->color_matrix[i][j - 1].b * ker[1][0] +
		image->color_matrix[i][j].b * ker[1][1] +
		image->color_matrix[i][j + 1].b * ker[1][2] +
		image->color_matrix[i + 1][j - 1].b * ker[2][0] +
		image->color_matrix[i + 1][j].b * ker[2][1] +
		image->color_matrix[i + 1][j + 1].b * ker[2][2];

		if (apply_type == 4) {
			red /= 16; blue /= 16; green /= 16;
			color.r = clamp_apply(red);
			color.g = clamp_apply(green);
			color.b = clamp_apply(blue);
		} else if (apply_type == 3) {
			red /= 9; blue /= 9; green /= 9;
			color.r = clamp_apply(red);
			color.g = clamp_apply(green);
			color.b = clamp_apply(blue);
		} else {
			color.r = clamp_apply(red);
			color.g = clamp_apply(green);
			color.b = clamp_apply(blue);
		}
	}
	return color;
}

unsigned char clamp_apply(int result)
{
	if (result > 255)
		result = 255;
	if (result < 0)
		result = 0;
	return (unsigned char)result;
}
