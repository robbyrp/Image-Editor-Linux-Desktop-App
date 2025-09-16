// Functions that generate the histogram of the image

#include "def.h"

void histogram(image_t *image)
{
	if (!image->color_matrix && !image->greyscale_matrix) {
		printf("No image loaded\n");
		return;
	}
	// CHECK FOR PARAMETERS
	char *aux = strtok(NULL, "\n ");
	if (!aux) {
		printf("Invalid command\n");
		return;
	}
	int x = atoi(aux);
	aux = strtok(NULL, "\n ");
	if (!aux) {
		printf("Invalid command\n");
		return;
	}
	int bin = atoi(aux);

	if (bin % 2 != 0 || bin > 256 || bin <= 0) {
		printf("Invalid set of parameters\n");
		return;
	}
	if (image->color_matrix && !image->greyscale_matrix) {
		printf("Black and white image needed\n");
		return;
	}

	histogram_greyscale(image, x, bin);

}

void histogram_greyscale(image_t *image, int x, int bin)
{
	// ARRAY OF FREQUENCES
	unsigned long long freq[256] = {0};
	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			freq[image->greyscale_matrix[i][j]]++;
		}
	}

	// SPLITTING THE FREQ[] ARRAY INTO SUMS OF BIN TERMS
	// AND CALCULATE THE MAX FREQUENCY
	int magic_number = 256 / bin;
	unsigned long long max_freq = 0;
	for (int i = 0; i < bin; i++) {
		for (int j = i * magic_number + 1; j < (i + 1)  * magic_number; j++) {
			freq[magic_number * i] += freq[j];
		}
		if (max_freq < freq[magic_number * i]) {
			max_freq = freq[magic_number * i];
		}
	}
	if (max_freq == 0) {
		printf("Image contains no data\n");
		return;
	}

	// PRINT AND CALCULATE NUMBER OF STARS
	for (int i = 0; i < bin; i++) {
		int stars = (freq[magic_number * i] * x) / max_freq;
		printf("%d\t|\t", stars);
		for (int j = 0; j < stars; j++) {
			printf("*");
		}
		printf("\n");
	}
}
