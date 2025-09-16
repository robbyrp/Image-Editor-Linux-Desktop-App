#include "def.h"

int main(void)
{
	image_t image;
	image.greyscale_matrix = NULL;
	image.color_matrix = NULL;
	selection_t selection;

	while (1) {
		char line[50] = {0};

		fgets(line, 50, stdin);

		char *input = strtok(line, "\n ");

		if (strcmp(input, "LOAD") == 0)
			load(&image, &selection);
		else if (strcmp(input, "SAVE") == 0)
			save(&image);
		else if (strcmp(input, "EXIT") == 0) {
			exit_program(&image);
			break;
		} else if (strcmp(input, "SELECT") == 0)
			select_region(&image, &selection);
		else if (strcmp(input, "ROTATE") == 0)
			rotate_region(&image, &selection);
		else if (strcmp(input, "EQUALIZE") == 0)
			equalize(&image);
		else if (strcmp(input, "CROP") == 0)
			crop_region(&image, &selection);
		else if (strcmp(input, "APPLY") == 0)
			apply(&image, &selection);
		else if (strcmp(input, "HISTOGRAM") == 0)
			histogram(&image);
		else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
