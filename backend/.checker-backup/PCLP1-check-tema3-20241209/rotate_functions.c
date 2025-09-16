// Functions that rotate the image 

#include "def.h"

void rotate_square_greyscale_90deg(image_t *image, selection_t *select)
{
	int x_start = select->x_start;
	int y_start = select->y_start;
	int x_end = select->x_end;
	int y_end = select->y_end;

	// TRANSPOSE THE MATRIX
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start + (i - y_start); j < x_end; j++) {
			int coord_i = y_start + (j - x_start);
			int coord_j = x_start + (i - y_start);
			int tmp = image->greyscale_matrix[i][j];
			image->greyscale_matrix[i][j] =
			image->greyscale_matrix[coord_i][coord_j];
			image->greyscale_matrix[coord_i][coord_j] = tmp;
		}
	}
	// SWAP COLS
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_start + (x_end - x_start) / 2; j++) {
			int mirror_col = x_end - 1 - (j - x_start);

			int tmp = image->greyscale_matrix[i][j];

			image->greyscale_matrix[i][j] =
			image->greyscale_matrix[i][mirror_col];

			image->greyscale_matrix[i][mirror_col] = tmp;
		}
	}

}

void rotate_square_color_90deg(image_t *image, selection_t *select)
{
	int x_start = select->x_start;
	int y_start = select->y_start;
	int x_end = select->x_end;
	int y_end = select->y_end;

	// TRANSPOSE THE MATRIX
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start + (i - y_start); j < x_end; j++) {
			pixel_t tmp;
			int coord_i = y_start + (j - x_start);
			int coord_j = x_start + (i - y_start);

			tmp = image->color_matrix[i][j];
			image->color_matrix[i][j] = image->color_matrix[coord_i][coord_j];
			image->color_matrix[coord_i][coord_j] = tmp;
		}
	}
	// SWAP COLS
	for (int i = y_start; i < y_end; i++) {
		for (int j = x_start; j < x_start + (x_end - x_start) / 2; j++) {
			pixel_t tmp;
			int mirror_col = x_end - 1 - (j - x_start);

			tmp = image->color_matrix[i][j];
			image->color_matrix[i][j] = image->color_matrix[i][mirror_col];
			image->color_matrix[i][mirror_col] = tmp;
		}
	}

}

void rotate_square(image_t *image, selection_t *select, int degrees)
{
	if (image->greyscale_matrix) {
		switch (degrees) {
		case 90:
			rotate_square_greyscale_90deg(image, select);
			break;
		case 180:
			for (int i = 0; i < 2; i++) {
				rotate_square_greyscale_90deg(image, select);
			}
			break;
		case 270:
			for (int i = 0; i < 3; i++) {
				rotate_square_greyscale_90deg(image, select);
			}
			break;
		case -90:
			for (int i = 0; i < 3; i++) {
				rotate_square_greyscale_90deg(image, select);
			}
			break;
		case -180:
			for (int i = 0; i < 2; i++) {
				rotate_square_greyscale_90deg(image, select);
			}
			break;
		case -270:
			rotate_square_greyscale_90deg(image, select);
			break;
		default:
			break;
		}
	}

	else if (image->color_matrix) {
		switch (degrees) {
		case 90:
			rotate_square_color_90deg(image, select);
			break;
		case 180:
			for (int i = 0; i < 2; i++)
				rotate_square_color_90deg(image, select);
			break;
		case 270:
			for (int i = 0; i < 3; i++)
				rotate_square_color_90deg(image, select);
			break;
		case -90:
			for (int i = 0; i < 3; i++)
				rotate_square_color_90deg(image, select);
			break;
		case -180:
			for (int i = 0; i < 2; i++)
				rotate_square_color_90deg(image, select);
			break;
		case -270:
			rotate_square_color_90deg(image, select);
			break;
		}
	}
	printf("Rotated %d\n", degrees);
}

void rotate_all_90deg(image_t *image, selection_t *select)
{
	// NEW MATRIX WITH SWAPPED DIMENSIONS
	image_t rotated;
	// SHALOW COPY, BUT WITH SWAPPED DIMENSIONS
	rotated = *image;
	rotated.rows = image->cols;
	rotated.cols = image->rows;
	rotated.greyscale_matrix = NULL;
	rotated.color_matrix = NULL;

	// GREYSCALE IMAGE
	if (image->greyscale_matrix) {

		alloc_greyscale(&rotated);

		// ROTATE 90DEGREES
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				rotated.greyscale_matrix[j][image->rows - 1 - i] =
					image->greyscale_matrix[i][j];
			}
		}

		// FREE OLD MATRIX AND UPDATE
		free_greyscale(image);
		*image = rotated;
	}

	// COLOR
	if (image->color_matrix) {

		alloc_color(&rotated);

		// ROTATE 90DEG
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				rotated.color_matrix[j][image->rows - 1 - i].r =
				image->color_matrix[i][j].r;
				rotated.color_matrix[j][image->rows - 1 - i].g =
				image->color_matrix[i][j].g;
				rotated.color_matrix[j][image->rows - 1 - i].b =
				image->color_matrix[i][j].b;
			}
		}

	 // FREE OLD MATRIX AND UPDATE

		free_color(image);
		*image = rotated;
	}

	// Update selection coordinates for the rotated image
	select->x_start = 0;
	select->y_start = 0;
	select->x_end = rotated.cols;
	select->y_end = rotated.rows;
}

void rotate_all(image_t *image, selection_t *select, int degrees)
{
	switch (degrees) {
	case 90:
		rotate_all_90deg(image, select);
		break;
	case 180:
		for (int i = 0; i < 2; i++) {
			rotate_all_90deg(image, select);
		}
		break;
	case 270:
		for (int i = 0; i < 3; i++) {
			rotate_all_90deg(image, select);
		}
		break;
	case -90:
		for (int i = 0; i < 3; i++) {
			rotate_all_90deg(image, select);
		}
		break;
	case -180:
		for (int i = 0; i < 2; i++) {
			rotate_all_90deg(image, select);
		}
		break;
	case -270:
		rotate_all_90deg(image, select);
		break;
	default:
		break;
	}
	printf("Rotated %d\n", degrees);

}

void rotate_region(image_t *image, selection_t *select)
{
	if (!image->greyscale_matrix && !image->color_matrix) {
		printf("No image loaded\n");
		return;
	}

	char *aux = strtok(NULL, "\n ");
	int degrees = atoi(aux);

	if (degrees % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	if (degrees % 360 == 0) {
		printf("Rotated %d\n", degrees);
		return;
	}

	if (!select->all) {
		rotate_square(image, select, degrees);
	}
	if (select->all) {
		rotate_all(image, select, degrees);
	}

}
