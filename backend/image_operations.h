#ifndef IMAGE_OPERATIONS_H
#define IMAGE_OPERATIONS_H

#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif
	void equalize(image_t *image);

	int check_selection(image_t *image, selection_t *select);
	void select_all(image_t *image, selection_t *select);
	void crop_region(image_t *image, selection_t *select);

	void rotate_square_greyscale_90deg(image_t *image, selection_t *select);
	void rotate_square_color_90deg(image_t *image, selection_t *select);
	void rotate_square(image_t *image, selection_t *select, int degrees);
	void rotate_all_90deg(image_t *image, selection_t *select);
	void rotate_all(image_t *image, selection_t *select, int degrees);
	bool check_square_selection(image_t *image, selection_t *select);

	void apply_sharpen(image_t *image, selection_t *select);
	void apply_edge(image_t *image, selection_t *select);
	void apply_blur(image_t *image, selection_t *select);
	void apply_gaussian_blur(image_t *image, selection_t *select);
	void apply_edge_enhance(image_t *image, selection_t *select);
	void apply_ridge_detection(image_t *image, selection_t *select);
	void apply_horizontal_prewitt(image_t *image, selection_t *select);
	void apply_vertical_prewitt(image_t *image, selection_t *select); 

#ifdef __cplusplus
}
#endif

#endif