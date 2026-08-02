#include "editor_state.h"
#include <stdlib.h>
#include <string.h>

    TextureState::TextureState() {
        texture_id = 0;
        display_buffer = nullptr;       
        convert = true;                 // if conversion to rgba is needed
        generate_texture = true;
    }

    TextureState::~TextureState() {
        if (display_buffer) {
            free(display_buffer);
            display_buffer = nullptr;
        }
    }

    ImageState::ImageState() {
        image = (image_t *)malloc(sizeof(image_t));
        if (image) {
            image->greyscale_matrix = nullptr;
            image->color_matrix = nullptr;
            image->greyscale_memblock = nullptr;
            image->color_memblock = nullptr;
            image->loaded = false;
            image->cols = 0;
            image->rows = 0;
            image->maxval = 255;
            strcpy(image->format, "");
        }

        backup_image = (image_t *)malloc(sizeof(image_t));
        if (backup_image) {
            backup_image->greyscale_matrix = nullptr;
            backup_image->color_matrix = nullptr;
            backup_image->greyscale_memblock = nullptr;
            backup_image->color_memblock = nullptr;
            backup_image->loaded = false;
            backup_image->cols = 0;
            backup_image->rows = 0;
            backup_image->maxval = 255;
            strcpy(backup_image->format, "");
            
        }

        selection = (selection_t *)malloc(sizeof(selection_t));
        if (selection) {
            selection->all = false;
            selection->x_start = 0;
            selection->y_start = 0;
            selection->x_end = 0;
            selection->y_end = 0;
        }

        backup_selection = (selection_t *)malloc(sizeof(selection_t));
        if (backup_selection) {
            backup_selection->all = false;
            backup_selection->x_start = 0;
            backup_selection->y_start = 0;
            backup_selection->x_end = 0;
            backup_selection->y_end = 0;
        }

        input_file_path = (char *)malloc(512 * sizeof(char));
        output_file_path = (char *)malloc(512 * sizeof(char));
        strcpy(input_file_path, "../backend");
        strcpy(output_file_path, "../backend");
    }

    ImageState::~ImageState() {
        if (image) {
            free_greyscale(image);
            free_color(image);
            free(image);
            image = nullptr;
        }
        if (backup_image) {
            free_greyscale(backup_image);
            free_color(backup_image);
            free(backup_image);
            backup_image = nullptr;
        }
        if (selection) {
            free(selection);
            selection = nullptr;
        }
        if (backup_selection) {
            free(backup_selection);
            backup_selection = nullptr;
        }
        free(input_file_path);
        free(output_file_path);
    }
    