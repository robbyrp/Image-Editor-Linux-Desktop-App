#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif
    void load_cli(image_t *image, selection_t *select);
	void load_gui(image_t *image, selection_t *select, const char *filename);
	bool load_memory_binary_gui(image_t *image, selection_t *select, const char* buffer);

    void save_cli(image_t *image);
	void save_ascii_gui(image_t *image, const char *new_filename);
	void save_binary_gui(image_t *image, const char *new_filename);
	bool is_binary(image_t *image);
#ifdef __cplusplus
}
#endif

#endif