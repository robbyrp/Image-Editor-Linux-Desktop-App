// Functions that save the new image that was created/processed

#include "def.h"

void save_ascii_gui(image_t *image, const char *new_filename)
{
	// OPEN THE FILE
	FILE *file = fopen(new_filename, "wt");
	fprintf(file, "%s\n", image->format);
	fprintf(file, "%d %d\n", image->cols, image->rows);
	fprintf(file, "%d\n", image->maxval);

	// P2 FORMAT
	if (strcmp(image->format, "P2") == 0) {
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				fprintf(file, "%d ", (int)image->greyscale_matrix[i][j]);
			}
			fprintf(file, "\n");
		}
	}
	// P3 FORMAT
	if (strcmp(image->format, "P3") == 0) {
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				// CAST BACK TO INT TO PRINT THE SAME VALUES
				fprintf(file, "%d ", (int)image->color_matrix[i][j].r);
				fprintf(file, "%d ", (int)image->color_matrix[i][j].g);
				fprintf(file, "%d ", (int)image->color_matrix[i][j].b);
			}
			fprintf(file, "\n");
		}
	}

	// CLOSE THE FILE
	fclose(file);
}

void save_binary_gui(image_t *image, const char *new_filename)
{
	// OPEN THE FILE
	FILE *file = fopen(new_filename, "wb");

	// WRITE THE HEADER IN ASCII USING FPRINTF
	fprintf(file, "%s\n", image->format);
	fprintf(file, "%d %d\n", image->cols, image->rows);
	fprintf(file, "%d\n", image->maxval);

	// WRITE THE ARRAY IN BINARY USING FWRITE
	// P5 GREYSCALE FORMAT
	if (strcmp(image->format, "P5") == 0) {
		if (!image->greyscale_matrix) {
			fprintf(stderr, "Greyscale_matrix is NULL\n");
			return;
		}
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				fwrite(&image->greyscale_matrix[i][j], sizeof(unsigned char),
					   1, file);
			}
		}
	}
	//P6 RGB FORMAT
	if (strcmp(image->format, "P6") == 0) {
		if (!image->color_matrix) {
			fprintf(stderr, "Color_matrix is NULL\n");
			return;
		}
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				fwrite(&image->color_matrix[i][j].r, sizeof(unsigned char),
					   1, file);
				fwrite(&image->color_matrix[i][j].g, sizeof(unsigned char),
					   1, file);
				fwrite(&image->color_matrix[i][j].b, sizeof(unsigned char),
					   1, file);
			}
		}
	}
	// CLOSE THE FILE
	fclose(file);
}

bool is_binary(image_t *image)
{
    if (strcmp(image->format, "P6") == 0 || strcmp(image->format, "P5") == 0) {
		return true;
	}
	return false;
}