// This program only works with P2 P3 P5 P6 formatted images!
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "../backend/def.h"
#include "funcs.h" // Functions for image display

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


// Main code
int main(int, char**)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;


	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	// #define WINDOW_WIDTH 1280
	// #define WINDOW_HEIGHT 1080

	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	const GLFWvidmode* return_struct = glfwGetVideoMode(glfwGetPrimaryMonitor());
	float window_width = (float)return_struct->width;
	float window_height = (float)return_struct->height;

	// float window_width = glfwGetMonitor
	GLFWwindow* window = glfwCreateWindow((int)(window_width * main_scale), (int)(window_height * main_scale), "Image Editor", nullptr, nullptr);
	glfwMaximizeWindow(window);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	
	// Background color
	ImVec4 clear_color =  ImVec4(0.9647f, 0.9098f, 0.9686f, 0.7f);
	
	// Button color
	ImVec4 button_color = ImVec4(0.9647f, 0.9098f, 0.9686f, 0.05f);
	ImGuiStyle& button_style = ImGui::GetStyle();
	button_style.Colors[ImGuiCol_Button] = button_color;
	button_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(button_color.x, button_color.y, button_color.z, 0.12f);
	button_style.Colors[ImGuiCol_ButtonActive] = ImVec4(button_color.x, button_color.y, button_color.z, 0.18f);
	// Combo button color
	style.Colors[ImGuiCol_FrameBg] = button_color;
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(button_color.x, button_color.y, button_color.z, 0.12f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(button_color.x, button_color.y, button_color.z, 0.18f);
	
	// Main loop

	// Initialize a pointer to a texture_state object
	static TextureState *t_state = new TextureState();
	
	// Initialize a pointer to an image_state object
	static ImageState *img_state = new ImageState();
			
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			// Top toolbar
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20)); // Main menu bar
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));  // Buttons
			ImGui::BeginMainMenuBar();
			
			load_button_logic(img_state, t_state);
			ImGui::SameLine();

			save_button_logic(img_state, t_state);
			ImGui::SameLine();

			selection_combo_logic(img_state);

			ImGui::PopStyleVar(2);
			ImGui::EndMainMenuBar();

			// Image display window
			display_image_window_logic(img_state, t_state);

			// Selection window
			selection_window_logic(img_state, t_state);

			// Sidebar menu window
			sidebar_menu_logic(img_state, t_state);

		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// Free the state object pointers
	delete t_state; 
	delete img_state;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
