#pragma once
#include "../backend/def.h"


unsigned char* convert_to_display_format(image_t *image);
void display_image(image_t *image, bool *conversion_needed, unsigned char **display_buffer);
