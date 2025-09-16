#include "funcs.h"
#include "../backend/sources/def.h"


// Function to convert image to RGBA for opengl display
unsigned char* convert_to_display_format(image_t *image) 
{
    if (!image || !image->loaded) {
        return nullptr;
    }
    
    int total_pixels = image->rows * image->cols;
    unsigned char *rgba_data = (unsigned char*)malloc(total_pixels * 4);
    
    if (!rgba_data) {
        printf("Failed to allocate memory for display buffer\n");
        return nullptr;
    }
    
    if (strcmp(image->format, "P2") == 0 || strcmp(image->format, "P5") == 0) {
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
    } else if (strcmp(image->format, "P3") == 0 || strcmp(image->format, "P6") == 0) {
        // Color image -> RGBA
        for (int i = 0; i < image->rows; i++) {
            for (int j = 0; j < image->cols; j++) {
                int idx = (i * image->cols + j) * 4;
                rgba_data[idx + 0] = image->color_matrix[i][j].r;
                rgba_data[idx + 1] = image->color_matrix[i][j].g;
                rgba_data[idx + 2] = image->color_matrix[i][j].b;
                rgba_data[idx + 3] = 255;   // Alpha
            }
        }
    }
    
    return rgba_data;
}

void display_image(image_t *image, bool *conversion_needed, unsigned char **display_buffer)
{
     if (image->loaded && *conversion_needed) {
        if (*display_buffer) {
            free(*display_buffer);
            *display_buffer = nullptr;
        }
        *display_buffer = convert_to_display_format(image);
        if (*display_buffer) {
            *conversion_needed = false;
        } else {
            printf("Image conversion failed\n");
        }
    }
}