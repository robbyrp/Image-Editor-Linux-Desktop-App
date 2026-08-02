#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H

#include "editor_state.h"

namespace Graphics {
    unsigned char* convert_to_display_format(image_t *image);
    void create_buffer(image_t *image, TextureState *t_state);
    void upload_texture_to_gpu(image_t *image, TextureState *t_state);
    void refresh_image_render(image_t *image, TextureState *t_state);
}


#endif
