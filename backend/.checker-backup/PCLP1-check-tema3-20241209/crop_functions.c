// Functions that crop the image

#include "def.h"

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

bool has_letters(char *aux)
{
	while (*aux) {
		if (((int)*aux >= 'A' && (int)*aux <= 'Z') ||
			((int)*aux >= 'a' && (int)*aux <= 'z')) {
			return true;
		}
		aux++;
	}
	return false;
}

void select_region(image_t *image, selection_t *select)
{
	if (!image->color_matrix && !image->greyscale_matrix) {
		printf("No image loaded\n");
		return;
	}
	char *aux = strtok(NULL, "\n ");
	if (!aux) {
		printf("Invalid command\n");
		return;
	}

	// SELECT ALL
	if (strcmp(aux, "ALL") == 0) {
		select->all = true;
		select->x_start = 0;
		select->y_start = 0;
		select->x_end = image->cols;
		select->y_end = image->rows;
		printf("Selected ALL\n");
		return;
	}

	// IF SELECTION IS NOT GOOD, KEEP THE PREVIOUS ONE
	selection_t temp_select;
	temp_select.all = false;

	//COORDINATES
	char *x_start_tok = aux;
	char *y_start_tok = strtok(NULL, "\n ");
	if (!y_start_tok) {
		printf("Invalid command\n");
		return;
	}
	char *x_end_tok = strtok(NULL, "\n ");
	if (!x_end_tok) {
		printf("Invalid command\n");
		return;
	}
	char *y_end_tok = strtok(NULL, "\n ");
	// does not function properly
	if (has_letters(y_end_tok) || has_letters(x_end_tok) ||
		has_letters(y_start_tok) || has_letters(x_start_tok)) {
		printf("Invalid command\n");
		return;
	}
	if (!y_end_tok) {
		printf("Invalid command\n");
		return;
	}

	aux = strtok(NULL, "\n ");
	if (aux) {
		// 5th ARGUMENT
		printf("Invalid command\n");
		return;
	}

	// CONVERT TO INTEGERS
	temp_select.x_start = atoi(x_start_tok);
	temp_select.y_start = atoi(y_start_tok);
	temp_select.x_end = atoi(x_end_tok);
	temp_select.y_end = atoi(y_end_tok);

	// KEEP OLD COORDINATES IF UNVALID
	if (check_selection(image, &temp_select) == 1) {
		printf("Invalid set of coordinates\n");
		return;
	}
	// SELECTION IS VALID
	*select = temp_select;
	// IF SELECTED COORDS COVER THE ENTIRE IMAGE
	if (select->x_start == 0 && select->y_start == 0 &&
		select->y_end == image->rows && select->x_end == image->cols) {
		select->all = true;
	}
	printf("Selected %d %d %d %d\n", select->x_start, select->y_start,
		   select->x_end, select->y_end);

}

void alloc_cropped_greyscale(image_t *cropped)
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

void alloc_cropped_color(image_t *cropped)
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
		for (int i = select->y_start; i < select->y_end; i++) {
			for (int j = select->x_start; j < select->x_end; j++) {
				// ROWS AND COLS OF CROPPED MATRIX
				int row = i - select->y_start;
				int col = j - select->x_start;
				cropped.greyscale_matrix[row][col] =
				image->greyscale_matrix[i][j];
			}
		}
		// FREE IMAGE POINTER
		free_greyscale(image);
	}

	// LOADED COLOR MATRIX
	if (image->color_matrix && !image->greyscale_matrix) {

		// MALLOC COLOR CROPPED MATRIX
		alloc_cropped_color(&cropped);

		// ASSIGN VALUES
		for (int i = select->y_start; i < select->y_end; i++) {
			for (int j = select->x_start; j < select->x_end; j++) {
				// ROWS AND COLS OF CROPPED MATRIX
				int row = i - select->y_start;
				int col = j - select->x_start;
				cropped.color_matrix[row][col].r =
				image->color_matrix[i][j].r;
				cropped.color_matrix[row][col].g =
				image->color_matrix[i][j].g;
				cropped.color_matrix[row][col].b =
				image->color_matrix[i][j].b;
			}
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
	printf("Image cropped\n");
}

