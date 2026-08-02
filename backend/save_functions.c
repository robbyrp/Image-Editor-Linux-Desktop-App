// Functions that save the new image that was created/processed

#include "image.h"

static void write_p2_row(FILE *file, const unsigned char *row, int cols);
static void write_p3_row(FILE *file, const pixel_t *row, int cols);

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
			write_p2_row(file, image->greyscale_matrix[i], image->cols);
		}
	}

	// P3 FORMAT
	if (strcmp(image->format, "P3") == 0) {
		for (int i = 0; i < image->rows; i++) {
			write_p3_row(file, image->color_matrix[i], image->cols);
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

	// P5 GREYSCALE FORMAT
	if (strcmp(image->format, "P5") == 0) {
		if (!image->greyscale_matrix) {
			fprintf(stderr, "Greyscale_matrix is NULL\n");
			return;
		}
		for (int i = 0; i < image->rows; i++) {
			fwrite(&image->greyscale_matrix[i], sizeof(unsigned char), image->cols, file);
		}
	}

	//P6 RGB FORMAT
	if (strcmp(image->format, "P6") == 0) {
		if (!image->color_matrix) {
			fprintf(stderr, "Color_matrix is NULL\n");
			return;
		}
		for (int i = 0; i < image->rows; i++) {
			fwrite(&image->color_matrix[i], sizeof(pixel_t), image->cols, file);
		}
	}

	// CLOSE THE FILE
	fclose(file);
}

static void write_p2_row(FILE *file, const unsigned char *row, int cols)
{
    size_t cap = (size_t)cols * ASCII_PIXEL_ESTIMATE + 2;
    char *buffer = malloc(cap);
    if (!buffer) {
        return;
    }

    char *cursor = buffer;
    size_t remaining = cap;

    for (int j = 0; j < cols; j++) {
        int written = snprintf(cursor, remaining, "%u ", row[j]);
        cursor += written;
        remaining -= (size_t)written;
    }

    if (cursor != buffer) {
        cursor[-1] = '\n';
        *cursor = '\0';
    } else {
        buffer[0] = '\n';
        buffer[1] = '\0';
    }

    fputs(buffer, file);
    free(buffer);
}

static void write_p3_row(FILE *file, const pixel_t *row, int cols)
{
    size_t cap = (size_t)cols * 3 * ASCII_PIXEL_ESTIMATE + 2;
    char *buffer = malloc(cap);
    if (!buffer) {
        return;
    }

    char *cursor = buffer;
    size_t remaining = cap;

    for (int j = 0; j < cols; j++) {
        int written = snprintf(cursor, remaining, "%u %u %u ",
                               row[j].r, row[j].g, row[j].b);
        cursor += written;
        remaining -= (size_t)written;
    }

    if (cursor != buffer) {
        cursor[-1] = '\n';
        *cursor = '\0';
    } else {
        buffer[0] = '\n';
        buffer[1] = '\0';
    }

    fputs(buffer, file);
    free(buffer);
}


