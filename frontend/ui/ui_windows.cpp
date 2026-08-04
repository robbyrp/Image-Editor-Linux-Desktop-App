#include "ui_windows.h"
#include "ui_callbacks.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../backend/image_operations.h"

namespace Ui {

	static void draw_image_widget(EditorContext *ctx);
	static void draw_sidebar_operations(EditorContext *cxt);
	static void draw_square_selection_popup_error(EditorContext *ctx);
	static void draw_selection_combo(EditorContext *ctx);
	static bool canDrawImageContainer(EditorContext *ctx);
	static bool canDrawSelectionContainer(EditorContext *ctx);
	static bool canDrawImageWidget(EditorContext *ctx);

	/**
	 * Draws the container window for the image. Displays the image
	 * by calling the draw_image_widget() method.
	 */
	void draw_image_container(EditorContext *ctx) 
	{
		if (!canDrawImageContainer(ctx)) return;

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
		ImGui::Text("Format: %s", ctx->img_state->image->format);
		ImGui::Text("Dimensions: %d x %d", ctx->img_state->image->cols, ctx->img_state->image->rows);
		ImGui::Separator();

		// Draw only the image widget conditionally; keep container visible.
		if (canDrawImageWidget(ctx)) {
			draw_image_widget(ctx);
		}

		ImGui::End();
	}

	/**
	 * Displays the top toolbar. The button callbacks are from ui_callbacks.cpp
	 */
	void draw_top_toolbar(EditorContext *ctx) 
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20)); // Main menu bar
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));  // Buttons
		ImGui::BeginMainMenuBar();
		
		if (ImGui::Button("Load from disk", ImVec2(160, 40))) {
			Ui::load_button_logic(ctx);
		}
		ImGui::SameLine();

		if (ImGui::Button("Save As", ImVec2(120, 40))) {
			Ui::save_button_logic(ctx);
		}
		ImGui::SameLine();

		if (ImGui::Button("Fetch dog", ImVec2(120, 40))) {
			Ui::fetch_dog_button_logic(ctx);
		}
		ImGui::SameLine();

		if (ImGui::Button("Undo", ImVec2(120, 40))) {
			Ui::undo_button_logic(ctx);
		}
		ImGui::SameLine();

		Ui::draw_selection_combo(ctx);

		ImGui::PopStyleVar(2);
		ImGui::EndMainMenuBar();
	}

	/**
	 * Draws the selection container and updates the selection flags.
	 */
	void draw_selection_container(EditorContext *ctx)
	{
		if (!canDrawSelectionContainer(ctx)) return;

		// Calculate position for selection window
		ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
		ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

		float selection_window_width = kRightPanelWidth;
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
		
		ctx->img_state->selection->all = false;

		// X coordinates on same row
		ImGui::PushItemWidth(80);
		if (ImGui::InputInt("##x_start",
			 &ctx->img_state->selection->x_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
			ctx->img_state->selection->changed = true;
		}
		ImGui::SameLine();
		if (ImGui::InputInt("##x_end",
			 &ctx->img_state->selection->x_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
			ctx->img_state->selection->changed = true;
		}

		ImGui::PopItemWidth();
		ImGui::Text("x_start       x_end");
		
		// Y coordinates on same row
		ImGui::PushItemWidth(80);
		if (ImGui::InputInt("##y_start",
			 &ctx->img_state->selection->y_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
			ctx->img_state->selection->changed = true;
		}
		ImGui::SameLine();
		if (ImGui::InputInt("##y_end",
			 &ctx->img_state->selection->y_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
			ctx->img_state->selection->changed = true;
		}

		ImGui::PopItemWidth();
		ImGui::Text("y_start       y_end");
		
		// If the user changed selection
		if (ctx->img_state->selection->changed && ctx->img_state->selection->all) {
			ctx->img_state->selection->all = false;
		}

		// Check selection
		if (check_selection(ctx->img_state->image, ctx->img_state->selection) == 1) {
			ImGui::TextColored(ImVec4(1,0,0,1), "Invalid selection!");
		}
		ImGui::End();
		}

	/**
	 * Draws the sidebar container and calls the method that handles
	 * all the sidebar button logic.
	 */
	void draw_sidebar_container(EditorContext *ctx)
	{
		if (!ctx->img_state->image->loaded) {
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
		
		Ui::draw_sidebar_operations(ctx);

		ImGui::End();
	}

	/**
	 * Draws the actual image from the context on the screen.
	 * Is called by draw_image_container
	 */
	static void draw_image_widget(EditorContext *ctx)
	{
		Graphics::upload_texture_to_gpu(ctx->img_state->image, ctx->t_state);

		if (ctx->t_state->texture_id) {
			// Scale the image with regards to the available space
			ImVec2 content_region = ImGui::GetContentRegionAvail();

			// Calculate a scale factor
			float scale_x = content_region.x / (float)ctx->img_state->image->cols;
			float scale_y = content_region.y / (float)ctx->img_state->image->rows;
			// Use the smaller scale
			float scale = (scale_x < scale_y) ? scale_x : scale_y;

			if (scale < 0.1f) scale = 0.1f;
			if (scale > 2.0f) scale = 2.0f;

			// Calculate new scaled dimensions
			ImVec2 scaled_size;
			scaled_size.x = (float)ctx->img_state->image->cols * scale;
			scaled_size.y = (float)ctx->img_state->image->rows * scale;

			// Center the div :)
			ImVec2 empty_space(content_region.x - scaled_size.x, content_region.y - scaled_size.y);
			ImVec2 cursor_pos = ImGui::GetCursorPos();
			if (empty_space.x > 0.0f) {
				cursor_pos.x += empty_space.x * 0.5f;
			}
			if (empty_space.y > 0.0f) {
				cursor_pos.y += empty_space.y * 0.5f;
			}
			ImGui::SetCursorPos(cursor_pos);

			ImGui::Image((void *)(intptr_t)ctx->t_state->texture_id, scaled_size);
		}
	}
	
	/**
	 * Draws the selection onClick event that extends a combo menu.
	 * Is called by the draw_top_toolbar()
	 */
	static void draw_selection_combo(EditorContext *ctx)
	{
		float window_width = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + window_width - 120.0f);

		// Selection combo box
		ImGui::SetNextItemWidth(300.0f);

		if (ImGui::BeginCombo("#select_combo", "Select", ImGuiComboFlags_None)) {
			if (ImGui::Selectable("Select All")) {
				Ui::handle_select_all_action(ctx);
			}
        ImGui::EndCombo();
    	}
	}

	/**
	 * Function that draws the sidebar buttons.
	 * Is called by draw_sidebar_container()
	 */
	static void draw_sidebar_operations(EditorContext *ctx)
	{
		if (ImGui::Button("Crop", ImVec2(-1, 40))) {
			Ui::crop_button_logic(ctx);
		}

		if (ImGui::Button("Rotate left", ImVec2(-1, 40))) {
			Ui::rotate_left_button_logic(ctx);
		}
		if (ImGui::Button("Rotate right", ImVec2(-1, 40))) {
			Ui::rotate_right_button_logic(ctx);
		}

		Ui::draw_square_selection_popup_error(ctx);

		if (image_is_greyscale(ctx->img_state->image)) {

			if (ImGui::Button("Equalize", ImVec2(-1, 40))) {
					Ui::apply_image_operation(ctx, equalize, ctx->img_state->image);
				}
		}
		

		// Kernel operations - only for color images
		if (image_is_color(ctx->img_state->image)) {
			if (ImGui::Button("Sharpen", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_sharpen, ctx->img_state->image, ctx->img_state->selection);
			}
			if (ImGui::Button("Edge detect", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_edge, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Box blur", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_blur, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Gaussian blur", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_gaussian_blur, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Edge enhance", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_edge_enhance, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Ridge detection", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_ridge_detection, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Horizontal detect", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_horizontal_prewitt, ctx->img_state->image, ctx->img_state->selection);				
			}
			if (ImGui::Button("Vertical detect", ImVec2(-1, 40))) {
				Ui::apply_image_operation(ctx, apply_vertical_prewitt, ctx->img_state->image, ctx->img_state->selection);				
			}
		}
	}

	/**
	 * Function that displays a popup error. 
	 * Is called by draw_selection_container()
	 */
	static void draw_square_selection_popup_error(EditorContext *ctx)
	{
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
	}

	static bool canDrawImageContainer(EditorContext *ctx)
	{
		return ctx && ctx->img_state && ctx->img_state->image && ctx->img_state->image->loaded;
	}

	static bool canDrawSelectionContainer(EditorContext *ctx)
	{
		return canDrawImageContainer(ctx);
	}

	static bool canDrawImageWidget(EditorContext *ctx)
	{
		if (!canDrawImageContainer(ctx) || !ctx->t_state)
			return false;

		if (!ctx->t_state->display_buffer || ctx->t_state->convert)
			return false;

		return true;
	}
}
