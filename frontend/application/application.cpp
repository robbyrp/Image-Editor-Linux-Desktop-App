#include "application.h"
#include "ui_windows.h"
#include "ui_callbacks.h"
#include "../rendering/rendering_engine.h"
#include "../backend/network.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace Ui {

		Application::Application() {
				window = nullptr;
				context.img_state = nullptr;
				context.t_state = nullptr;
				clear_color = ImVec4(0.9647f, 0.9098f, 0.9686f, 0.7f);
		}

		Application::~Application() {}

		bool Application::init(int width, int height, const char *title) 
		{
			glfwSetErrorCallback(glfw_error_callback);
			if (!glfwInit()) return false;

			if (network_init() != 0) {
					fprintf(stderr, "Error initializing network module\n");
					glfwTerminate();
					return false;
			}

			float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

			window = glfwCreateWindow((int)(width * main_scale), (int)(height * main_scale), title, nullptr, nullptr);
    	glfwMaximizeWindow(window);
			if (window == nullptr)
					return false;
			glfwMakeContextCurrent(window);
			glfwSwapInterval(1); // Enable vsync

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
	
			context.img_state = new ImageState();
			context.t_state = new TextureState();

			return true;
		}

		void Application::run() 
		{
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
						Ui::draw_top_toolbar(&context);
						Ui::draw_image_container(&context);
						Ui::draw_selection_container(&context);
						Ui::draw_sidebar_container(&context);
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
			
			}

		void Application::shutdown() {
				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplGlfw_Shutdown();
				ImGui::DestroyContext();

				delete context.t_state;
				delete context.img_state;

				glfwDestroyWindow(window);
				glfwTerminate();

				network_cleanup();
		}

}
