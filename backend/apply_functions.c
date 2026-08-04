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

static pixel_t get_pixel_clamped(const image_t *image, int r, int c);

static pixel_t get_pixel_clamped(const image_t *image, int r, int c)
{
    int clamped_r = r;
    int clamped_c = c;

    if (clamped_r < 0) {
        clamped_r = 0;
    } else if (clamped_r >= image->rows) {
        clamped_r = image->rows - 1;
    }

    if (clamped_c < 0) {
        clamped_c = 0;
    } else if (clamped_c >= image->cols) {
        clamped_c = image->cols - 1;
    }

    return image->color_matrix[clamped_r][clamped_c];
}

static pixel_t apply_kernel_pixel(image_t *image,
                                  const int ker[3][3],
                                  int i, int j,
                                  int divisor)
{
    int red_sum = 0;
    int green_sum = 0;
    int blue_sum = 0;

    for (int m = -1; m <= 1; m++) {
        for (int n = -1; n <= 1; n++) {
            pixel_t neighbor = get_pixel_clamped(image, i + m, j + n);
            
            int weight = ker[m + 1][n + 1];

            red_sum   += neighbor.r * weight;
            green_sum += neighbor.g * weight;
            blue_sum  += neighbor.b * weight;
        }
    }

    if (divisor != 1) {
        red_sum /= divisor;
        green_sum /= divisor;
        blue_sum /= divisor;
    }

    pixel_t out;
    out.r = clamp_apply(red_sum);
    out.g = clamp_apply(green_sum);
    out.b = clamp_apply(blue_sum);
    return out;
}

/**
 * Uses a multithreading preprocessing directive called
 *     #pragma omp parallel for
 */
static void apply_filter_color(image_t *image, selection_t *select, const int ker[3][3], int divisor)
{

    image_t temp_image;
    temp_image = *image;
    temp_image.color_matrix = NULL;
    temp_image.greyscale_matrix = NULL;

    alloc_color(&temp_image);

    size_t total_bytes = image->rows * image->cols * sizeof(pixel_t);
    memcpy(temp_image.color_memblock, image->color_memblock, total_bytes);

    #pragma omp parallel for
    for (int i = select->y_start; i < select->y_end; i++) {
        for (int j = select->x_start; j < select->x_end; j++) {
            pixel_t result = apply_kernel_pixel(image, ker, i, j, divisor);
            temp_image.color_matrix[i][j] = result;
        }
    }

    free_color(image);
    // Copy the temp_image struct from the stack to the heap
    *image = temp_image;
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

