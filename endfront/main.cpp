// This program only works with P2 P3 P5 P6 formatted images!
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "backend/def.h"
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
    #define WINDOW_WIDTH 1280
    #define WINDOW_HEIGHT 1080
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(WINDOW_WIDTH * main_scale), (int)(WINDOW_HEIGHT * main_scale), "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
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
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
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

    // Initialize a colored image
    static image_t image = {0};
    image.loaded = false;
    // Initialize a selection
    static selection_t selected_region = {0};

    // Convert image for display purposes variables
    static bool conversion_needed = true;
    static unsigned char *display_buffer = nullptr;
    static GLuint texture_id = 0;

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
            static char input_file_path[512] = "../backend/";  // Default input path
            static char output_file_path[512] = "../backend/";  // Default output path
            

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20)); // Main menu bar
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));  // Buttons
            ImGui::BeginMainMenuBar();
            
            // Load button
            if (ImGui::Button("Load", ImVec2(120, 40))) {
                // Use zenity to navigate file system
                FILE* fp = nullptr;
                fp = popen("which zenity > /dev/null 2>&1", "r");
                if (fp && pclose(fp) == 0) {
                    fp = popen("zenity --file-selection --file-filter='PPM/PGM Images | *.ppm *.pgm' --title='Select Image'", "r");
                    
                    if (fp) {
                        char loading_file_path[512];
                        // fgets reads the selected_file_path from zenity's stdout
                        if (fgets(loading_file_path, sizeof(loading_file_path), fp)) {
                            // Remove newline from end of path
                            loading_file_path[strcspn(loading_file_path, "\n")] = 0;
                            if (strlen(loading_file_path) > 0) {
                                strcpy(input_file_path, loading_file_path);
                                load_gui(&image, &selected_region, input_file_path);
                                image.loaded = true;
                                conversion_needed = true;
                            }
                        }
                        pclose(fp);
                        // Display the image
                        display_image(&image, &conversion_needed, &display_buffer);

                    }
                } else {
                    printf("You need to install zenity first!\n");
                }
            }
            ImGui::SameLine();

            // Save button
            if (ImGui::Button("Save As", ImVec2(120, 40))) {
                if (image.loaded) {
                    FILE *fp = nullptr;

                    // Navigate file system using zenity
                    fp = popen("which zenity > /dev/null 2>&1", "r");
                    if (fp && pclose(fp) == 0) {
                        const char *ext = ".ppm"; // default
                        if (is_binary(&image))
                            ext = ".ppm";
                        if (!is_binary(&image))
                            ext = ".pgm";

                        // Get the command
                        char zenity_cmd[512];
                        snprintf(zenity_cmd, sizeof(zenity_cmd), 
                        "zenity --file-selection --save --confirm-overwrite --filename='output%s' --title='Save Image As'", ext);
                        fp = popen(zenity_cmd, "r");

                        if (fp) {
                            // fgets reads the saved_file_path from zenity's stdout
                            char saved_file_path[512];
                            if (fgets(saved_file_path, sizeof(saved_file_path), fp)) {
                                // Remove newline from end of path
                                saved_file_path[strcspn(saved_file_path, "\n")] = 0;
                                if (strlen(saved_file_path) > 0) {
                                    strcpy(output_file_path, saved_file_path);
                                    if (is_binary(&image))
                                        save_binary_gui(&image, output_file_path);
                                    else
                                        save_ascii_gui(&image, output_file_path);
                                    printf("File saved successfully to %s\n", output_file_path);
                                }
                            }
                            pclose(fp);
                        }
                    } else {
                        printf("You need to install Zenity first!\n");
                    }
                } else {
                    printf("You need to load a file first!\n");
                }
            }
            ImGui::SameLine();

           float window_width = ImGui::GetContentRegionAvail().x;
           ImGui::SetCursorPosX(ImGui::GetCursorPosX() + window_width - 120.0f);

            // Selection combo box
            ImGui::SetNextItemWidth(300.0f);

            if (ImGui::BeginCombo("#select_combo", "Select", ImGuiComboFlags_None)) {
                if (ImGui::Selectable("Select All")) {
                    if (image.loaded) {
                        select_all(&image, &selected_region);
                        selected_region.all = true;
                    }
                }
                
                ImGui::EndCombo();
            }

            ImGui::PopStyleVar(2);
            ImGui::EndMainMenuBar();

            // Display image window
            if (image.loaded && display_buffer && !conversion_needed) {
                // Calculate position
                ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
                ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

                // Left side, under taskbar
                float taskbar_height = 60.0f;
                ImVec2 window_pos = ImVec2(main_viewport_pos.x, main_viewport_pos.y + taskbar_height);

                // Fixed width, height based on avail space
                float window_width = 1200.0f;
                float window_height = main_viewport_size.y - taskbar_height;
                ImVec2 window_size = ImVec2(window_width, window_height);

                // Set window pos and size(fixed)
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

                // Create window
                ImGui::Begin("Image Display", nullptr, ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

                ImGui::Text("Image Info:");
                ImGui::Text("Format: %s", image.format);
                ImGui::Text("Dimensions: %d x %d", image.cols, image.rows);
                ImGui::Separator();

                // Create OpenGL texture from buffer
                if (texture_id == 0) {
                    glGenTextures(1, &texture_id);
                }
                    glBindTexture(GL_TEXTURE_2D, texture_id);

                    // Set texture parameters
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    // Upload texture data
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, display_buffer);
                
                // Scale the image with regards to the available space
                ImVec2 content_region = ImGui::GetContentRegionAvail();

                // Calculate a scale factor
                float scale_x = content_region.x / (float)image.cols;
                float scale_y = content_region.y / (float)image.rows;
                // Use the smaller scale
                float scale = (scale_x < scale_y) ? scale_x : scale_y;

                if (scale < 0.1f) scale = 0.1f;
                if (scale > 2.0f) scale = 2.0f;

                // Calculate new scaled dimensions
                ImVec2 scaled_size;
                scaled_size.x = (float)image.cols * scale;
                scaled_size.y = (float)image.rows * scale;

                // Display image
                ImGui::Image((void *)(intptr_t)texture_id, scaled_size);
                ImGui::End();
            }

            // Selection tab
            if (image.loaded && display_buffer && !conversion_needed) {
                // Calculate position for selection window
                ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
                ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

                float taskbar_height = 60.0f;
                float selection_window_width = 250.0f;
                float selection_window_x = main_viewport_size.x - selection_window_width;

                ImVec2 selection_pos = ImVec2(selection_window_x, main_viewport_pos.y + taskbar_height + 10.0f);
                ImVec2 selection_size = ImVec2(selection_window_width, 130.0f);
                
                ImGui::SetNextWindowPos(selection_pos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(selection_size, ImGuiCond_Always);
                
                ImGui::Begin("Selection Coordinates", nullptr, 
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove | 
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoTitleBar);

                // The defaults
                static int x_start = 0, x_end = image.cols;
                static int y_start = 0, y_end = image.rows;

                // Auto complete if "Select All button" pressed
                if (selected_region.all) {
                    x_start = 0;
                    x_end = image.cols;
                    y_start = 0;
                    y_end = image.rows;
                }

                ImGui::Text("Selection:");
                ImGui::Separator();
                
                selected_region.changed = false;
                // X coordinates on same row
                ImGui::PushItemWidth(80);
                if (ImGui::InputInt("##x_start", &x_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
                    selected_region.changed = true;
                }
                ImGui::SameLine();
                if (ImGui::InputInt("##x_end", &x_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
                    selected_region.changed = true;
                }
                ImGui::PopItemWidth();
                ImGui::Text("x_start       x_end");
                
                // Y coordinates on same row
                ImGui::PushItemWidth(80);
                if (ImGui::InputInt("##y_start", &y_start, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
                    selected_region.changed = true;
                }
                ImGui::SameLine();
                if (ImGui::InputInt("##y_end", &y_end, 0, 0, ImGuiInputTextFlags_CharsDecimal)) {
                    selected_region.changed = true;
                }
                ImGui::PopItemWidth();
                ImGui::Text("y_start       y_end");
                
                // If the user changed selection
                if (selected_region.changed && selected_region.all) {
                    selected_region.all = false; 
                }
                // Assign inputted values to selection region
                selected_region.x_start = x_start;
                selected_region.x_end = x_end;
                selected_region.y_start = y_start;
                selected_region.y_end = y_end;
                
                // Check selection
                if (check_selection(&image, &selected_region) == 1) {
                    ImGui::TextColored(ImVec4(1,0,0,1), "Invalid selection!");
                }
                ImGui::End();
            }
            
            // Sidebar menu
            if (image.loaded) {
                ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
                ImVec2 main_viewport_size = ImGui::GetMainViewport()->Size;

                float sidebar_width = 200.0f;
                float sidebar_height = main_viewport_size.y / 2.0f - 10.0f;
                float sidebar_x = main_viewport_pos.x + main_viewport_size.x - sidebar_width;
                float sidebar_y = main_viewport_pos.y + main_viewport_size.y / 2.0f;

                ImGui::SetNextWindowPos(ImVec2(sidebar_x, sidebar_y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(sidebar_width, sidebar_height), ImGuiCond_Always);
            
                ImGui::Begin("Sidebar", nullptr, 
                             ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoMove | 
                             ImGuiWindowFlags_NoCollapse);
                
                ImGui::Text("Operations:");
                ImGui::Separator();

                // Crop and rotate operations
                static bool show_square_popup = false;
                if (ImGui::Button("Crop", ImVec2(-1, 40))) {
                    crop_region(&image, &selected_region);
                    conversion_needed = true;
                    display_image(&image, &conversion_needed, &display_buffer);
                }
                if (ImGui::Button("Rotate left", ImVec2(-1, 40))) {
                    if (selected_region.x_end - selected_region.x_start != selected_region.y_end - selected_region.y_start) {
                        show_square_popup = true;
                    } else {
                        rotate_square(&image, &selected_region, 90);
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                }
                if (ImGui::Button("Rotate right", ImVec2(-1, 40))) {
                    if (selected_region.x_end - selected_region.x_start != selected_region.y_end - selected_region.y_start) {
                        show_square_popup = true;
                    } else {
                        rotate_square(&image, &selected_region, -90);
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                }
                // Popup for square selection error
                if (show_square_popup) {
                    ImGui::OpenPopup("ErrorSquare");
                }
                if (ImGui::BeginPopupModal("ErrorSquare", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Selection must be squared!");
                    if (ImGui::Button("OK")) {
                        show_square_popup = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                // Equalize operation - only for greyscale images
                if (image.greyscale_matrix && !image.color_matrix) {
                    if (ImGui::Button("Equalize", ImVec2(-1, 40))) {
                        equalize_greyscale(&image);
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                }

                // Kernel operations - only for color images
                if (!image.greyscale_matrix && image.color_matrix) {
                    if (ImGui::Button("Sharpen", ImVec2(-1, 40))) {
                        apply_sharpen(&image, &selected_region);
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                    if (ImGui::Button("Edge detect", ImVec2(-1, 40))) {
                        apply_edge(&image, &selected_region);
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                    if (ImGui::Button("Box blur", ImVec2(-1, 40))) {
                        for (int i = 0; i < 4; i++) {
                            apply_blur(&image, &selected_region);
                        }
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }
                    if (ImGui::Button("Gaussian blur", ImVec2(-1, 40))) {
                        for (int i = 0; i < 4; i++) {
                            apply_gaussian_blur(&image, &selected_region);
                        }
                        conversion_needed = true;
                        display_image(&image, &conversion_needed, &display_buffer);
                    }

                }


                ImGui::End();
            }
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

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
