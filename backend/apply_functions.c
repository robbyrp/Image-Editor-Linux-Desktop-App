// Functions that apply different filters

#include "image_operations.h"

static pixel_t apply_kernel_pixel(image_t *image,
                                  const int ker[3][3],
                                  int i, int j,
                                  int divisor);
                                  
static void apply_filter_color(image_t *image,
                                selection_t *select,
                                const int ker[3][3],
                                int divisor);

static unsigned char clamp_apply(int result);

static pixel_t apply_kernel_pixel(image_t *image,
                                  const int ker[3][3],
                                  int i, int j,
                                  int divisor)
{
    if (i == 0 || j == 0 || i == image->rows - 1 || j == image->cols - 1) {
        return image->color_matrix[i][j];
    }

    int red =
        image->color_matrix[i - 1][j - 1].r * ker[0][0] +
        image->color_matrix[i - 1][j].r     * ker[0][1] +
        image->color_matrix[i - 1][j + 1].r * ker[0][2] +
        image->color_matrix[i][j - 1].r     * ker[1][0] +
        image->color_matrix[i][j].r         * ker[1][1] +
        image->color_matrix[i][j + 1].r     * ker[1][2] +
        image->color_matrix[i + 1][j - 1].r * ker[2][0] +
        image->color_matrix[i + 1][j].r     * ker[2][1] +
        image->color_matrix[i + 1][j + 1].r * ker[2][2];

    int green = 
		image->color_matrix[i - 1][j - 1].g * ker[0][0] +
        image->color_matrix[i - 1][j].g     * ker[0][1] +
        image->color_matrix[i - 1][j + 1].g * ker[0][2] +
        image->color_matrix[i][j - 1].g     * ker[1][0] +
        image->color_matrix[i][j].g         * ker[1][1] +
        image->color_matrix[i][j + 1].g     * ker[1][2] +
        image->color_matrix[i + 1][j - 1].g * ker[2][0] +
        image->color_matrix[i + 1][j].g     * ker[2][1] +
        image->color_matrix[i + 1][j + 1].g * ker[2][2];
 
    int blue  = 
		image->color_matrix[i - 1][j - 1].b * ker[0][0] +
        image->color_matrix[i - 1][j].b     * ker[0][1] +
        image->color_matrix[i - 1][j + 1].b * ker[0][2] +
        image->color_matrix[i][j - 1].b     * ker[1][0] +
        image->color_matrix[i][j].b         * ker[1][1] +
        image->color_matrix[i][j + 1].b     * ker[1][2] +
        image->color_matrix[i + 1][j - 1].b * ker[2][0] +
        image->color_matrix[i + 1][j].b     * ker[2][1] +
        image->color_matrix[i + 1][j + 1].b * ker[2][2];


    if (divisor != 1) {
        red /= divisor;
        green /= divisor;
        blue /= divisor;
    }

    pixel_t out;
    out.r = clamp_apply(red);
    out.g = clamp_apply(green);
    out.b = clamp_apply(blue);
    return out;
}

static void apply_filter_color(image_t *image, selection_t *select, const int ker[3][3], int divisor)
{
    image_t new_image = *image;
    new_image.color_matrix = NULL;
    new_image.greyscale_matrix = NULL;

    alloc_color(&new_image);

    for (int i = 0; i < image->rows; i++) {
        memcpy(new_image.color_matrix[i],
               image->color_matrix[i],
               (size_t)image->cols * sizeof(pixel_t));
    }

    for (int i = select->y_start; i < select->y_end; i++) {
        for (int j = select->x_start; j < select->x_end; j++) {
            new_image.color_matrix[i][j] = apply_kernel_pixel(image, ker, i, j, divisor);
        }
    }

    free_color(image);
    *image = new_image;
}

static unsigned char clamp_apply(int result)
{
	if (result > 255)
		result = 255;
	if (result < 0)
		result = 0;
	return (unsigned char)result;
}

void apply_sharpen(image_t *image, selection_t *select)
{
	int ker[3][3] = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};

	apply_filter_color(image, select, ker, 1);
}

void apply_edge(image_t *image, selection_t *select)
{
	int ker[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}
	};

	apply_filter_color(image, select, ker, 1);
}

void apply_blur(image_t *image, selection_t *select)
{
	int ker[3][3] = {
		{1, 1, 1},
		{1, 1, 1,},
		{1, 1, 1}
	};

	apply_filter_color(image, select, ker, 9);

}

void apply_gaussian_blur(image_t *image, selection_t *select)
{
	int ker[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};

	apply_filter_color(image, select, ker, 16);
	
}

void apply_edge_enhance(image_t *image, selection_t *select) 
{
    int ker[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    apply_filter_color(image, select, ker, 1);
}


void apply_box_blur(image_t *image, selection_t *select) 
{
    int ker[3][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1}
    };
    apply_filter_color(image, select, ker, 9);
}

void apply_ridge_detection(image_t *image, selection_t *select) 
{
    int ker[3][3] = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}
    };
    apply_filter_color(image, select, ker, 1);
}

void apply_horizontal_prewitt(image_t *image, selection_t *select) 
{
    int ker[3][3] = {
        {-1, -1, -1},
        {0, 0, 0},
        {1, 1, 1}
    };
    apply_filter_color(image, select, ker, 3);
}

void apply_vertical_prewitt(image_t *image, selection_t *select) 
{
    int ker[3][3] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    apply_filter_color(image, select, ker, 3);
}

