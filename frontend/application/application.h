#ifndef APPLICATION_H
#define APPLICATION_H

#include "editor_state.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

namespace Ui {
    
class Application {
private:
    GLFWwindow* window;
    EditorContext context;
    ImVec4 clear_color;

public:
    Application();
    ~Application();

    bool init(int width, int height, const char *title);

    void run();

    void shutdown();
};

}

#endif
