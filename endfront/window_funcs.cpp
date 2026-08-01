#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "../backend/image.h"
#include "../backend/image_operations.h"
#include "funcs.h" // Functions for image display

static constexpr float kTaskbarHeight = 60.0f;
static constexpr float kTopMargin = 10.0f;
static constexpr float kRightPanelWidth = 200.0f;
static constexpr float kRightPanelGap = 14.0f;
static constexpr float kSelectionWindowHeight = 130.0f;
static constexpr float kSidebarTopOffset = 210.0f;

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

// Function to create or update the display buffer
void create_buffer(image_t *image, TextureState *t_state)
{
	if (image->loaded && t_state->convert) {
		if (t_state->display_buffer) {
			free(t_state->display_buffer);
			t_state->display_buffer = nullptr;
		}
		t_state->display_buffer = convert_to_display_format(image);
		if (t_state->display_buffer) {
			t_state->convert = false;
		} else {
			printf("Image conversion failed\n");
		}
	}
}

void create_opengl_texture(ImageState *img_state, TextureState *t_state)
{
	// Create OpenGL texture only when image is changed
	if (t_state->generate_texture) {
		if (t_state->texture_id == 0) {
		glGenTextures(1, &(t_state->texture_id));
		}
		glBindTexture(GL_TEXTURE_2D, t_state->texture_id);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Upload texture data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_state->image->cols, img_state->image->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, t_state->display_buffer);
	
		// Reset the flag
		t_state->generate_texture = false;
	}
	
	// Always display the image if texture is available
	if (t_state->texture_id) {
		// Scale the image with regards to the available space
		ImVec2 content_region = ImGui::GetContentRegionAvail();

		// Calculate a scale factor
		float scale_x = content_region.x / (float)img_state->image->cols;
		float scale_y = content_region.y / (float)img_state->image->rows;
		// Use the smaller scale
		float scale = (scale_x < scale_y) ? scale_x : scale_y;

		if (scale < 0.1f) scale = 0.1f;
		if (scale > 2.0f) scale = 2.0f;

		// Calculate new scaled dimensions
		ImVec2 scaled_size;
		scaled_size.x = (float)img_state->image->cols * scale;
		scaled_size.y = (float)img_state->image->rows * scale;

		ImGui::Image((void *)(intptr_t)t_state->texture_id, scaled_size);
	}
}

void image_window_display(ImageState *img_state, TextureState *t_state)
{
	if (!img_state->image->loaded || !t_state->display_buffer || t_state->convert) {
		return;
	}
	// Calculate position
	ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
	ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

	// Left side, under taskbar
	ImVec2 window_pos = ImVec2(main_viewport_pos.x, main_viewport_pos.y + kTaskbarHeight + kTopMargin);

	// Fixed width, height based on avail space
	float window_width = main_viewport_size.x - kRightPanelWidth - kRightPanelGap;
	if (window_width < 400.0f) window_width = 400.0f;
	float window_height = main_viewport_size.y - kTaskbarHeight - kTopMargin;
	ImVec2 window_size = ImVec2(window_width, window_height);

	// Set window pos and size(fixed)
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	// Create window
	ImGui::Begin("Image Display", nullptr, ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Image Info:");
	ImGui::Text("Format: %s", img_state->image->format);
	ImGui::Text("Dimensions: %d x %d", img_state->image->cols, img_state->image->rows);
	ImGui::Separator();

	// Create OpenGL texture only when image is changed
	create_opengl_texture(img_state, t_state);

	ImGui::End();
}

void selection_window_display(ImageState *img_state, TextureState *t_state)
{
	if (!img_state->image->loaded || !t_state->display_buffer || t_state->convert) {
		return;
	}

	// Calculate position for selection window
	ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
	ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

	float selection_window_width = kRightPanelWidth; // same as sidebar
	float selection_window_x = main_viewport_pos.x + main_viewport_size.x - selection_window_width;

	ImVec2 selection_pos = ImVec2(selection_window_x, main_viewport_pos.y + kTaskbarHeight + kTopMargin);
	ImVec2 selection_size = ImVec2(selection_window_width, kSelectionWindowHeight);
	
	ImGui::SetNextWindowPos(selection_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(selection_size, ImGuiCond_Always);
	
	ImGui::Begin("Selection Coordinates", nullptr, 
				ImGuiWindowFlags_NoResize | 
				ImGuiWindowFlags_NoMove | 
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("Selection:");
	ImGui::Separator();
	
	img_state->selection->all = false;

	// X coordinates on same row
	ImGui::PushItemWidth(80);
	if (ImGui::InputInt("##x_start", &img_state->selection->x_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
		img_state->selection->changed = true;
	}
	ImGui::SameLine();
	if (ImGui::InputInt("##x_end", &img_state->selection->x_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
		img_state->selection->changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::Text("x_start       x_end");
	
	// Y coordinates on same row
	ImGui::PushItemWidth(80);
	if (ImGui::InputInt("##y_start", &img_state->selection->y_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
		img_state->selection->changed = true;
	}
	ImGui::SameLine();
	if (ImGui::InputInt("##y_end", &img_state->selection->y_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
		img_state->selection->changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::Text("y_start       y_end");
	
	// If the user changed selection
	if (img_state->selection->changed && img_state->selection->all) {
		img_state->selection->all = false;
	}

	// Check selection
	if (check_selection(img_state->image, img_state->selection) == 1) {
		ImGui::TextColored(ImVec4(1,0,0,1), "Invalid selection!");
	}
	ImGui::End();
}

void top_toolbar_display(ImageState *img_state, TextureState *t_state) {
	// Top toolbar
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20)); // Main menu bar
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));  // Buttons
	ImGui::BeginMainMenuBar();
	
	load_button_logic(img_state, t_state);
	ImGui::SameLine();

	save_button_logic(img_state, t_state);
	ImGui::SameLine();

	fetch_dog_button_logic(img_state, t_state);
	ImGui::SameLine();

	undo_button_logic(img_state, t_state);
	ImGui::SameLine();

	selection_combo_logic(img_state);

	ImGui::PopStyleVar(2);
	ImGui::EndMainMenuBar();
}

void sidebar_menu_display(ImageState *img_state, TextureState *t_state)
{
	if (!img_state->image->loaded) {
		return;
	}

	// Calculate position for sidebar window
	ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
	ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

	float sidebar_y = main_viewport_pos.y + kSidebarTopOffset;
	float sidebar_width = kRightPanelWidth;
	float sidebar_height = main_viewport_size.y - (sidebar_y - main_viewport_pos.y);
	float sidebar_x = main_viewport_pos.x + main_viewport_size.x - sidebar_width;

	ImGui::SetNextWindowPos(ImVec2(sidebar_x, sidebar_y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(sidebar_width, sidebar_height), ImGuiCond_Always);

	ImGui::Begin("Sidebar", nullptr, 
					ImGuiWindowFlags_NoResize | 
					ImGuiWindowFlags_NoMove | 
					ImGuiWindowFlags_NoCollapse);
	
	ImGui::Text("Operations:");
	ImGui::Separator();
	
	sidebar_menu_logic(img_state, t_state);

	ImGui::End();
	
}
