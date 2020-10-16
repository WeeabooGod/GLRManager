#pragma once
#include <cstdio>
#include "../../Libraries/IMGui/imgui.h"
#include "../../Libraries/IMGui/imgui_impl_glfw.h"
#include "../../Libraries/IMGui/imgui_impl_opengl3.h"

#include <GL/gl3w.h>

#include <string>
#include <GLFW/glfw3.h>

#include "../Tools/FreeTypeFont.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

class ImguiOpenGL
{
    GLFWwindow* window;
    FreeTypeTest Freetype;
	bool FlushOldAtlas = false;
public:
    ImguiOpenGL(const std::string& programName);

    void SetupImGuiFrame();
    void RenderImGui();
    void CleanupImGuiGL();
	void UpdateFontAtlasThread(std::atomic_bool& done);
	
	void FreetypeInit();

    GLFWwindow* GetWindow() const;
};
