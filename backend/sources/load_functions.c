// Functions that handle the PPM or PGM image
// By reading pixels from the inputted file and loading them into memory

#include "def.h"

// Big load function handles both ascii and binary, greyscale or color
// By calling the necessary functions
// It also allocates memory for the images by calling the alloc functions

void load_gui(image_t *image, selection_t *select, const char *filename)
{
	// OPEN FILE
	FILE *in = fopen(filename, "rt");
	if (!in) {
		printf("Could not load %s\n", filename);
		// CLEAR CURRENT IMAGE
		if (image->greyscale_matrix) {
			free_greyscale(image);
		}
		if (image->color_matrix) {
			free_color(image);
		}
		select->all = false;
		select->x_start = 0;
		select->y_start = 0;
		select->x_end = 0;
		select->y_end = 0;
		return;
	}

	// READ THE FORMAT
	if (fscanf(in, "%s", image->format) != 1) {
		fclose(in);
		printf("Failed to load %s\n", filename);
		// CLEAR THE CURRENT IMAGE
		if (image->greyscale_matrix) {
			free_greyscale(image);
		}
		if (image->color_matrix) {
			free_color(image);
		}
		return;
	}
	// CLOSE THE FILE
	fclose(in);

		// FREE IF ALREADY ALLOCATED
	if (image->greyscale_matrix) {
		free_greyscale(image);
	}
	if (image->color_matrix) {
		free_color(image);
	}

	// FORMATS
	if (strcmp(image->format, "P1") == 0 ||
		strcmp(image->format, "P2") == 0 ||
		strcmp(image->format, "P3") == 0) {
		load_ascii(image, filename);
	}
	if (strcmp(image->format, "P4") == 0 ||
		strcmp(image->format, "P5") == 0 ||
		strcmp(image->format, "P6") == 0) {
		load_binary(image, filename);
	}
	// UPDATE SELECTION IF LOAD SUCCESSFULL
	if (image->greyscale_matrix || image->color_matrix) {
		select->all = true;
		select->x_start = 0;
		select->y_start = 0;
		select->x_end = image->cols;
		select->y_end = image->rows;
		printf("Loaded %s\n", filename);
	}

}
// --------------------- CLI AND GUI FUNCTIONS ---------------------
void load_ascii(image_t *image, const char *filename)
{
	// OPEN FILE
	FILE *in = fopen(filename, "rt");
	if (!in) {
		printf("Failed to load the text file  %s\n", filename);
		return;
	}

	// READ FIRST LINE AGAIN
	char ignore[1024];
	fscanf(in, "%s", ignore);

	// P2 GREYSCALE FORMAT
	if (strcmp(image->format, "P2") == 0) {
		// READ COLLS AND ROWS
		fscanf(in, "%d %d", &image->cols, &image->rows);

		// READ MAXVAL 255
		fscanf(in, "%d", &image->maxval);

		alloc_greyscale(image);

		// ASSIGN VALUES
		for (int i = 0 ; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				int value;
				fscanf(in, "%d", &value);
				// READ INT AND CAST TO UNSIGNED CHAR AFTERWARDS
				image->greyscale_matrix[i][j] = (unsigned char)value;
			}
		}

	}
	// P3 RGB FORMAT
	if (strcmp(image->format, "P3") == 0) {
		// READ COLLS AND ROWS
		fscanf(in, "%d %d", &image->cols, &image->rows);
		// READ MAXVAL 255
		fscanf(in, "%d", &image->maxval);

		alloc_color(image);

		//ASSIGN VALUES
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				int r, g, b;
				fscanf(in, "%d %d %d", &r, &g, &b);
				// READ INT AND CAST TO UNSIGNED CHAR AFTERWARDS
				image->color_matrix[i][j].r = (unsigned char)r;
				image->color_matrix[i][j].g = (unsigned char)g;
				image->color_matrix[i][j].b = (unsigned char)b;
			}
		}
	}
	// CLOSE THE FILE
	fclose(in);
}

void load_binary(image_t *image, const char *filename)
{
	// OPEN FILE
	FILE *in = fopen(filename, "rb");
	if (!in) {
		printf("Failed to load the binary file %s\n", filename);
		return;
	}

	// READ FIRST LINE AGAIN
	char ignore[1024];
	fscanf(in, "%s", ignore);

	// P5 GREYSCALE FORMAT
	if (strcmp(image->format, "P5") == 0) {
		fscanf(in, "%d %d", &image->cols, &image->rows);
		// READ MAXVAL 255
		fscanf(in, "%d\n", &image->maxval);

		alloc_greyscale(image);

		// ASSIGN VALUES
		for (int i = 0 ; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				fread(&image->greyscale_matrix[i][j],
					  sizeof(unsigned char), 1, in);
			}
		}
	}
	// P6 RGB FORMAT
	if (strcmp(image->format, "P6") == 0) {
		fscanf(in, "%d %d", &image->cols, &image->rows);
		// READ MAXVAL 255
		fscanf(in, "%d\n", &image->maxval);

		alloc_color(image);

		// ASSIGN VALUES
		for (int i = 0; i < image->rows; i++) {
			for (int j = 0; j < image->cols; j++) {
				fread(&image->color_matrix[i][j].r,
					  sizeof(unsigned char), 1, in);
				fread(&image->color_matrix[i][j].g,
					  sizeof(unsigned char), 1, in);
				fread(&image->color_matrix[i][j].b,
					  sizeof(unsigned char), 1, in);
			}
		}
	}
	// CLOSE THE FILE
	fclose(in);
}
