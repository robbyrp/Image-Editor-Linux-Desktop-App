#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "../backend/image.h"
#include <GL/gl.h>

class TextureState {
public:
    GLuint texture_id;
    unsigned char *display_buffer;
    bool convert;
    bool generate_texture;

    TextureState();
    ~TextureState();
};

class ImageState {
public:
    image_t *image;
    image_t *backup_image;
    selection_t *selection;
    selection_t *backup_selection;
    char *input_file_path;
    char *output_file_path;

    ImageState();
    ~ImageState();
};

struct EditorContext {
  ImageState *img_state;
  TextureState *t_state;  
};


#endif