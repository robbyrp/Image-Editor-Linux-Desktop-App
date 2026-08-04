#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include "ui_callbacks.h"
#include "../rendering/rendering_engine.h"
#include "../backend/network.h"
#include "../backend/image_operations.h"
#include "../backend/load_functions.h"

namespace Ui { 
	void load_button_logic(EditorContext *ctx) 
	{
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
		if (ctx->img_state->image->loaded) {
			free_greyscale(ctx->img_state->image);
			free_color(ctx->img_state->image);
			ctx->img_state->image->loaded = false;
		}
		if (ctx->t_state->display_buffer) {
			free(ctx->t_state->display_buffer);
			ctx->t_state->display_buffer = nullptr;
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
					strcpy(ctx->img_state->input_file_path, loading_file_path);
					Ui::save_state_for_undo(ctx);
					load_image_from_disk(ctx->img_state->image, ctx->img_state->selection, ctx->img_state->input_file_path);
					ctx->img_state->image->loaded = (image_is_greyscale(ctx->img_state->image) || image_is_color(ctx->img_state->image));
			}
		}

		pclose(fp);
		if (ctx->img_state->image->loaded) 
			Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
  }

	void save_button_logic(EditorContext *ctx)
	{
		if (!ctx->img_state->image->loaded) {
			return;
		}
		
		// Use zenity to navigate file system
		FILE *fp = nullptr;
		fp = popen("which zenity > /dev/null 2>&1", "r");

		if (!fp && pclose(fp) != 0) {
			return;
		}

		const char *ext = ".ppm"; // Set a default extension aribitrarily
		if (is_binary(ctx->img_state->image))
			ext = ".ppm";
		if (!is_binary(ctx->img_state->image))
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
				strcpy(ctx->img_state->output_file_path, saved_file_path);
				if (is_binary(ctx->img_state->image))
					save_binary_gui(ctx->img_state->image, ctx->img_state->output_file_path);
				else
					save_ascii_gui(ctx->img_state->image, ctx->img_state->output_file_path);
			}
		}
		pclose(fp);
		
	}

	/**
	 * Makes a GET request for a shared memory key and formats the it by adding a '/' before.
	 * Calls the load_image_from_shm() function from the backend in order to access the shared memory
	 * and load the image directly from it.
	 */
	void fetch_dog_button_logic(EditorContext *ctx)
	{
		const char *RANDOM_BREED_BEST_SIZED_IMAGE_FROM_LIST = "/api/random-breed/best-sized-image-from-list";
		memory_struct_t chunk = http_get_request_content(RANDOM_BREED_BEST_SIZED_IMAGE_FROM_LIST);
		if (chunk.memory == NULL || chunk.size == 0) {
			fprintf(stderr, "http_get_request_content(RANDOM_BREED_BEST_SIZED_IMAGE_FROM_LIST) failed\n");
			return;
		}
		
		std::string raw_json(chunk.memory, chunk.size);
		std::string shm_key_string = "/" + raw_json.substr(1, raw_json.size() - 2);

		const char *shm_key = shm_key_string.c_str();		

		Ui::save_state_for_undo(ctx);

		if (!load_image_from_shm(ctx->img_state->image, ctx->img_state->selection, shm_key)) {
			fprintf(stderr, "Failed to load image from shared memory\n");
			ctx->img_state->image->loaded = false;
			free_chunk(&chunk);
			return;
		}

		ctx->img_state->image->loaded = true;
		Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
	
		free_chunk(&chunk);
	}

	void undo_button_logic(EditorContext *ctx)
	{    
		if (Ui::isImageLoaded(ctx)) {
				std::swap(ctx->img_state->selection, ctx->img_state->backup_selection);
				std::swap(ctx->img_state->image, ctx->img_state->backup_image);
				Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
			}
	}

	void crop_button_logic(EditorContext *ctx)
	{
		if (check_selection(ctx->img_state->image, ctx->img_state->selection) == 1) return;

		Ui::save_state_for_undo(ctx);
		select_all(ctx->img_state->backup_image, ctx->img_state->backup_selection);
		crop_region(ctx->img_state->image, ctx->img_state->selection);
		Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);	
	}

	void rotate_left_button_logic(EditorContext *ctx)
	{
		// TODO: Make backend function that checks square selection
		if (!check_square_selection(ctx->img_state->image, ctx->img_state->selection)) {
				ImGui::OpenPopup("Square Selection Error");
			} else {
				Ui::save_state_for_undo(ctx);
				rotate_square(ctx->img_state->image, ctx->img_state->selection, 90);
				Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
			}
	}

	void rotate_right_button_logic(EditorContext *ctx)
	{
		// TODO: Make backend function that checks square selection
		if (ctx->img_state->selection->x_end - ctx->img_state->selection->x_start !=
				ctx->img_state->selection->y_end - ctx->img_state->selection->y_start) {
				ImGui::OpenPopup("Square Selection Error");
			} else {
				Ui::save_state_for_undo(ctx);
				rotate_square(ctx->img_state->image, ctx->img_state->selection, -90);
				Graphics::refresh_image_render(ctx->img_state->image, ctx->t_state);
			}
	}

	void save_state_for_undo(EditorContext *ctx)
	{
		ctx->img_state->backup_image = clone_image(ctx->img_state->backup_image, ctx->img_state->image);
		*ctx->img_state->backup_selection = *ctx->img_state->selection;
	}	

	void handle_select_all_action(EditorContext *ctx)
{
    if (ctx->img_state->image->loaded) {
        select_all(ctx->img_state->image, ctx->img_state->selection);
        ctx->img_state->selection->all = true;
    }
}

	bool isImageLoaded(EditorContext *ctx)
	{
		if (!ctx || !ctx->img_state || !ctx->img_state->image) return false;
			return ctx->img_state->image->loaded;
	}

}
