#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "funcs.h"

static void refresh_image_render(ImageState *img_state, TextureState *t_state);

void load_button_logic(ImageState *img_state, TextureState *t_state)
{
	// Load button
	if (ImGui::Button("Load", ImVec2(120, 40))) {
		// Use zenity to navigate file system
		FILE* fp = nullptr;
		fp = popen("which zenity > /dev/null 2>&1", "r");
		
		if (!fp && pclose(fp) != 0) {
			ImGui::OpenPopup("Zenity Not Found");
			return;
		}

		fp = popen("zenity --file-selection --file-filter='PPM/PGM Images | *.ppm *.pgm' --title='Select Image'", "r");
		
		if (!fp && pclose(fp) != 0) {
			ImGui::OpenPopup("Zenity Not Found");
			return;
			
		}
		
		// Free the previous image and the buffer
		if (img_state->image->loaded) {
			free_greyscale(img_state->image);
			free_color(img_state->image);
			img_state->image->loaded = false;
		}
		if (t_state->display_buffer) {
			free(t_state->display_buffer);
			t_state->display_buffer = nullptr;
		}
		
		// Popup for zenity error
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImVec2 popup_size(400, 180);
		ImGui::SetNextWindowSize(popup_size, ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal("Zenity Not Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("You need to install Zenity first!");
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 30, 30, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 60, 60, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 20, 20, 255));
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(3);
			ImGui::EndPopup();
		}

		char loading_file_path[512];
		// fgets reads the selected_file_path from zenity's stdout
		if (fgets(loading_file_path, sizeof(loading_file_path), fp)) {
			// Remove newline from end of path
			loading_file_path[strcspn(loading_file_path, "\n")] = 0;
			if (strlen(loading_file_path) > 0) {
				strcpy(img_state->input_file_path, loading_file_path);
				img_state->image->loaded = true;
				t_state->convert = true;
				t_state->generate_texture = true;
				load_gui(img_state->image, img_state->selection, img_state->input_file_path);
			}
		}
		pclose(fp);
		// Display the image
		create_buffer(img_state->image, t_state);
		// Set flag to true because function automatically sets it to false
		t_state->generate_texture = true;

	}
}

void save_button_logic(ImageState *img_state, TextureState *t_state)
{
	// Save button
	if (ImGui::Button("Save As", ImVec2(120, 40))) {
		if (!img_state->image->loaded) {
			return;
		}
		
		// Use zenity to navigate file system
		FILE *fp = nullptr;
		fp = popen("which zenity > /dev/null 2>&1", "r");

		if (!fp && pclose(fp) != 0) {
			return;
		}

		const char *ext = ".ppm"; // Set a default extension aribitrarily
		if (is_binary(img_state->image))
			ext = ".ppm";
		if (!is_binary(img_state->image))
			ext = ".pgm";

		// Get the command
		char zenity_cmd[512];
		snprintf(zenity_cmd, sizeof(zenity_cmd), 
		"zenity --file-selection --save --confirm-overwrite --filename='output%s' --title='Save Image As'", ext);
		fp = popen(zenity_cmd, "r");

		if (!fp) {
			return;
		}
		// fgets reads the saved_file_path from zenity's stdout
		char saved_file_path[512];
		if (fgets(saved_file_path, sizeof(saved_file_path), fp)) {
			// Remove newline from end of path
			saved_file_path[strcspn(saved_file_path, "\n")] = 0;
			if (strlen(saved_file_path) > 0) {
				strcpy(img_state->output_file_path, saved_file_path);
				if (is_binary(img_state->image))
					save_binary_gui(img_state->image, img_state->output_file_path);
				else
					save_ascii_gui(img_state->image, img_state->output_file_path);
			}
		}
		pclose(fp);
	}
}

void fetch_dog_button_logic(ImageState *img_state, TextureState *t_state) {
	if (ImGui::Button("Fetch dog", ImVec2(120, 40))) {
		const char *RANDOM_BREED_URL = "/api/random-breed";
		memory_struct_t chunk = http_get_request_content(RANDOM_BREED_URL);
		if (chunk.memory != NULL && chunk.size != 0) {
			if (!load_memory_binary_gui(img_state->image, img_state->selection, chunk.memory)) {
				fprintf(stderr, "Could not load the image from the memory buffer into the program's memory\n");
				return;
			}
			img_state->image->loaded = true;
			save_state_for_undo(img_state);
			refresh_image_render(img_state, t_state);
		}
		free_chunk(&chunk);
	}
}

void selection_combo_logic(ImageState *img_state)
{
	float window_width = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + window_width - 120.0f);

	// Selection combo box
	ImGui::SetNextItemWidth(300.0f);

	if (ImGui::BeginCombo("#select_combo", "Select", ImGuiComboFlags_None)) {
		if (ImGui::Selectable("Select All")) {
			if (img_state->image->loaded) {
				select_all(img_state->image, img_state->selection);
				img_state->selection->all = true;
			}
		}
		ImGui::EndCombo();
	}
}

void sidebar_menu_logic(ImageState *img_state, TextureState *t_state)
{
	// Crop and rotate operations

	if (ImGui::Button("Crop", ImVec2(-1, 40))) {
		if (check_selection(img_state->image, img_state->selection) == 1) {
			return;
		} else {
			save_state_for_undo(img_state);
			crop_region(img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
	}
	if (ImGui::Button("Rotate left", ImVec2(-1, 40))) {
		if (img_state->selection->x_end - img_state->selection->x_start !=
			img_state->selection->y_end - img_state->selection->y_start) {
			ImGui::OpenPopup("Square Selection Error");
		} else {
			save_state_for_undo(img_state);
			rotate_square(img_state->image, img_state->selection, 90);
			refresh_image_render(img_state, t_state);
		}
	}
	if (ImGui::Button("Rotate right", ImVec2(-1, 40))) {
		if (img_state->selection->x_end - img_state->selection->x_start !=
			img_state->selection->y_end - img_state->selection->y_start) {
			ImGui::OpenPopup("Square Selection Error");
		} else {
			save_state_for_undo(img_state);
			rotate_square(img_state->image, img_state->selection, -90);
			refresh_image_render(img_state, t_state);
		}
	}

	// Popup for square selection error
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImVec2 popup_size(400, 180);
	ImGui::SetNextWindowSize(popup_size, ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal("Square Selection Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Rotation requires a square selection!");
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) * 0.5f);
		ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 30, 30, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 60, 60, 255));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 20, 20, 255));
		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleColor(3);
		ImGui::EndPopup();
	}

	// Equalize operation - only for greyscale images
	if (img_state->image->greyscale_matrix && !img_state->image->color_matrix) {
		static double last_equalize_press = 0;
		double now = ImGui::GetTime();

		if (ImGui::Button("Equalize", ImVec2(-1, 40))) {
			if (now - last_equalize_press > 2.0) {
				save_state_for_undo(img_state);
				time_operation(equalize, img_state->image);
				refresh_image_render(img_state, t_state);
				last_equalize_press = now;
			}
		}
	}

	// Kernel operations - only for color images
	if (!img_state->image->greyscale_matrix && img_state->image->color_matrix) {
		if (ImGui::Button("Sharpen", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_sharpen, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Edge detect", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_edge, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Box blur", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_blur, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Gaussian blur", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_gaussian_blur, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Edge enhance", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_edge_enhance, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Ridge detection", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_ridge_detection, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Horizontal detect", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_horizontal_prewitt, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
		if (ImGui::Button("Vertical detect", ImVec2(-1, 40))) {
			save_state_for_undo(img_state);
			time_operation(apply_vertical_prewitt, img_state->image, img_state->selection);
			refresh_image_render(img_state, t_state);
		}
	}
}

void undo_button_logic(ImageState *img_state, TextureState *t_state)
{
	if (ImGui::Button("Undo", ImVec2(120, 40))) {
		if (img_state->backup_image->loaded) {
			std::swap(img_state->selection, img_state->backup_selection);
			std::swap(img_state->image, img_state->backup_image);
			time_operation(refresh_image_render, img_state, t_state);
		}
	}
}

void save_state_for_undo(ImageState *img_state)
{
	img_state->backup_image = clone_image(img_state->backup_image, img_state->image);
	*img_state->backup_selection = *img_state->selection;
}

static void refresh_image_render(ImageState *img_state, TextureState *t_state)
{
	t_state->convert = true;
	t_state->generate_texture = true;
	create_buffer(img_state->image, t_state);
}
