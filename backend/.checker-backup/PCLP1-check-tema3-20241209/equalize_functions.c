// Functions that apply the equalize filter

#include "def.h"

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

unsigned char clamp(double result)
{
	if (result > 255)
		result = 255;
	if (result < 0)
		result = 0;
	return (unsigned char)(result + 0.5);
}

void equalize_greyscale(image_t *image)
{
	#define GREY image->greyscale_matrix
	// FREQUENCE ARRAY
	int freq[256] = {0};
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			freq[GREY[i][j]]++;
		}
	}

	// AREA
	double area = (double)image->rows * image->cols;

	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			// SUM OF PREV FREQUENCIES
			double sum = 0;
			int prev_pixel = GREY[i][j];
			for (int k = 0; k <= prev_pixel; k++) {
				sum += freq[k];
			}
			double result = 255 * (1 / area) * sum;
			GREY[i][j] = clamp(result);
		}
	}
	#undef GREY
	printf("Equalize done\n");
}