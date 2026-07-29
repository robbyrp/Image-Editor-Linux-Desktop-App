#pragma once
#include "../backend/def.h"
#include <GL/gl.h>
#include <ctime>                // For timing operations
#include <utility>              // For std::forward

class TextureState {
public:
    GLuint texture_id;
    unsigned char *display_buffer;
    bool convert;
    bool generate_texture;

    // Constructor
    TextureState() {
        texture_id = 0;
        display_buffer = nullptr;       
        convert = true;                 // if conversion to rgba is needed
        generate_texture = true;
    }

    // Destructor
    ~TextureState() {
        if (display_buffer) {
            free(display_buffer);
            display_buffer = nullptr;
        }
    }
};

class ImageState {
public:
    image_t *image;
    selection_t *selection;
    char *input_file_path;
    char *output_file_path;

    // Constructor
    ImageState() {
        image = (image_t *)malloc(sizeof(image_t));
        if (image) {
            image->greyscale_matrix = nullptr;
            image->color_matrix = nullptr;
            image->loaded = false;
            image->cols = 0;
            image->rows = 0;
            image->maxval = 255;
            strcpy(image->format, "");
        }

        selection = (selection_t *)malloc(sizeof(selection_t));
        if (selection) {
            selection->all = false;
            selection->x_start = 0;
            selection->y_start = 0;
            selection->x_end = 0;
            selection->y_end = 0;
        }

        input_file_path = (char *)malloc(512 * sizeof(char));
        output_file_path = (char *)malloc(512 * sizeof(char));
        strcpy(input_file_path, "../backend");
        strcpy(output_file_path, "../backend");
    }

    // Destructor
    ~ImageState() {
        if (image) {
            free_greyscale(image);
            free_color(image);
            free(image);
            image = nullptr;
        }
        if (selection) {
            free(selection);
            selection = nullptr;
        }
        free(input_file_path);
        free(output_file_path);
    }
};

unsigned char* convert_to_display_format(image_t *image);
void create_buffer(image_t *image, TextureState *t_state);

void load_button_logic(ImageState *image, TextureState *t_state);
void save_button_logic(ImageState *img_state, TextureState *t_state);
void fetch_dog_button_logic(ImageState *img_state, TextureState *t_state);
void selection_combo_logic(ImageState *img_state);
void selection_window_display(ImageState *img_state, TextureState *t_state);

void top_toolbar_display(ImageState *img_state, TextureState *t_state);

void image_window_display(ImageState *img_state, TextureState *t_state);

void sidebar_menu_display(ImageState *img_state, TextureState *t_state);
void sidebar_menu_logic(ImageState *img_state, TextureState *t_state);
// void popup_message(const char *title, const char *message);


// Template function has to be defined in the header file
template <typename Func, typename... Args>
void time_operation(Func&& func, Args&&... args)
{
    clock_t start = clock();
    std::forward<Func>(func)(std::forward<Args>(args)...);
    clock_t end = clock();
    double duration_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    printf("Operation took %.2f ms\n", duration_ms);
}
