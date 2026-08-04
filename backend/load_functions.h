#ifndef LOAD_FUNCTIONS_H
#define LOAD_FUNCTIONS_H

#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif
    void load_cli(image_t *image, selection_t *select);
	void load_image_from_disk(image_t *image, selection_t *select, const char *filename);
	bool load_binary_image_from_buffer(image_t *image, selection_t *select, const char* buffer);
	bool load_image_from_shm(image_t *image, selection_t *select, const char *shm_key);

    void save_cli(image_t *image);
	void save_ascii_gui(image_t *image, const char *new_filename);
	void save_binary_gui(image_t *image, const char *new_filename);
	bool is_binary(image_t *image);
#ifdef __cplusplus
}
#endif

#endif