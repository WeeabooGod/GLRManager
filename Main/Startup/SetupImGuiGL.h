#pragma once
#include "../../Libraries/IMGui/imgui.h"
#include "../../Libraries/IMGui/imgui_impl_glfw.h"
#include "../../Libraries/IMGui/imgui_impl_opengl3.h"
#include <cstdio>

#include <GL/gl3w.h>
// Include glfw3.h after our OpenGL definitions
#include <string>
#include <GLFW/glfw3.h>


class ImguiOpenGL
{
    GLFWwindow* window;

public:
    ImguiOpenGL(const std::string& programName);

    void SetupImGuiFrame();
    void RenderImGui();
    void CleanupImGuiGL();

    GLFWwindow* GetWindow() const;
};
