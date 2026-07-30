// Functions that apply the equalize filter

#include "image_operations.h"

static void equalize_greyscale(image_t *image);

void equalize(image_t *image)
{
	if (!image->color_matrix && !image->greyscale_matrix) {
		printf("No image loaded\n");
		return;
	}
	if (image->color_matrix && !image->greyscale_matrix) {
		printf("Black and white image needed\n");
		return;
	} else if (!image->color_matrix && image->greyscale_matrix) {
		equalize_greyscale(image);
	}

}

static void equalize_greyscale(image_t *image)
{
    int hist[256] = {0};

    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            hist[image->greyscale_matrix[i][j]]++;
        }
    }

    int cdf[256];
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    int total = image->rows * image->cols;
    int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            int value = image->greyscale_matrix[i][j];
            double mapped = (double)(cdf[value] - cdf_min) * 255.0 / (double)(total - cdf_min);
            if (mapped < 0) mapped = 0;
            if (mapped > 255) mapped = 255;
            image->greyscale_matrix[i][j] = (unsigned char)(mapped + 0.5);
        }
    }
}
