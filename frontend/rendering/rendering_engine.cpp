#include "editor_state.h"
#include "rendering_engine.h"

#include <GL/gl.h>

namespace Graphics {
    /**
     * Converts an image_t image to RGBA. The result is passed to opengl
     * in order to be displayed.
     */
    unsigned char* convert_to_display_format(image_t *image) 
    {
        if (!image || !image->loaded) {
            return nullptr;
        }
        
        size_t total_pixels = (size_t)image->rows * (size_t)image->cols;
        unsigned char *rgba_data = (unsigned char *)malloc(total_pixels * 4);
        
        if (!rgba_data) {
            printf("Failed to allocate memory for display buffer\n");
            return nullptr;
        }
        
        if (image_is_greyscale(image)) {
            // Grayscale image to RGBA
            for (int i = 0; i < image->rows; i++) {
                for (int j = 0; j < image->cols; j++) {
                    int idx = (i * image->cols + j) * 4;
                    unsigned char gray = image->greyscale_matrix[i][j];
                    rgba_data[idx + 0] = gray;  // R
                    rgba_data[idx + 1] = gray;  // G
                    rgba_data[idx + 2] = gray;  // B
                    rgba_data[idx + 3] = 255;   // Alpha
                }
            }
        } else if (image_is_color(image)) {
            // Color image -> RGBA
            for (int i = 0; i < image->rows; i++) {
                for (int j = 0; j < image->cols; j++) {
                    int idx = (i * image->cols + j) * 4;
                    rgba_data[idx + 0] = image->color_matrix[i][j].r;
                    rgba_data[idx + 1] = image->color_matrix[i][j].g;
                    rgba_data[idx + 2] = image->color_matrix[i][j].b;
                    rgba_data[idx + 3] = 255;
                }
            }
        }
        
        return rgba_data;
    }

    /**
     * Functions that creates or updates the rgba display buffer
     */
    void create_buffer(image_t *image, TextureState *t_state)
    {
        if (!image->loaded || !t_state->convert) return;
        
        if (t_state->display_buffer) {
            free(t_state->display_buffer);
            t_state->display_buffer = nullptr;
        }
        
        t_state->display_buffer = convert_to_display_format(image);
        
        if (t_state->display_buffer) {
            t_state->convert = false;
        } else {
            fprintf(stderr, "Image conversion failed\n");
        }
    }

    /**
     * Loads the texture bytes to the graphics card.
     */
    void upload_texture_to_gpu(image_t *image, TextureState *t_state)
    {
        if (!t_state->generate_texture) return;

        // Create OpenGL texture only when image is changed
        if (t_state->texture_id == 0) {
            glGenTextures(1, &(t_state->texture_id));
        }
        glBindTexture(GL_TEXTURE_2D, t_state->texture_id);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Upload texture data onto the graphics card
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->cols, image->rows, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, t_state->display_buffer);

        // Reset flag
        t_state->generate_texture = false;
    }

    /**
     * Method that sets the t_state flags so that the image will be re-rendered 
     * by the graphics engine. Called on every operation that alters the image.
     */
    void refresh_image_render(image_t *image, TextureState *t_state)
    {
	    t_state->convert = true;
	    t_state->generate_texture = true;
	    create_buffer(image, t_state);
    }

}
