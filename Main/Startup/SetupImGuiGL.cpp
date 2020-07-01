#include "SetupImGuiGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../IMGui/stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


ImguiOpenGL::ImguiOpenGL(const std::string& programName)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    // Create window with graphics context
    window = glfwCreateWindow(1480, 720, programName.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    gl3wInit();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiCol_DragDropTarget;
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
	io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf",16.0);

	io.IniFilename = "Resources/DockConfig.ini";

	//Icon for program
	GLFWimage images[1];
	images[0].pixels = stbi_load("Resources/Images/Icon.png", &images[0].width, &images[0].height, nullptr, 4); //rgba channels
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

}

void ImguiOpenGL::SetupImGuiFrame()
{
	//Poll Events
	glfwPollEvents();
	
	//Freetype init
	FreetypeInit();

	//Once the thread Update Atlas Rebuild we need to flush the old data and create a new one
	if (FlushOldAtlas == true)
	{
		ImGui_ImplOpenGL3_DestroyDeviceObjects();
	    ImGui_ImplOpenGL3_CreateDeviceObjects();
		FlushOldAtlas = false;
	}
	
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
}

void ImguiOpenGL::RenderImGui()
{
    // Rendering
    ImGui::Render();
    auto display_w = 0;
    auto display_h = 0;
	
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    glfwSwapBuffers(window);
}

void ImguiOpenGL::CleanupImGuiGL()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}


void ImguiOpenGL::UpdateFontAtlasThread(std::atomic_bool& done)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImFontConfig config;
	config.MergeMode = true;
	auto* NewFonts = new ImFontAtlas;
	NewFonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf",16.0);
	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKjp-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesJapanese());
	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKkr-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesKorean());
	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKsc-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKtc-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesChineseFull());
	NewFonts->Build();

	//Freetype init
	NewFonts->TexGlyphPadding = 1;
	unsigned int FontsFlags;
	
	FontsFlags = ImGuiFreeType::ForceAutoHint;
	//FontsFlags |= ImGuiFreeType::Bold;
	
	for (int n = 0; n < NewFonts->ConfigData.Size; n++)
	{
		auto* font_config = static_cast<ImFontConfig*>(&NewFonts->ConfigData[n]);
		font_config->RasterizerMultiply = 1.0f;
		font_config->RasterizerFlags = FontsFlags;
	}
	ImGuiFreeType::BuildFontAtlas(NewFonts, FontsFlags);

	//This is so that if we close prematurely we can use the bool to not try to access the data
	if (done == false)
	{
		io.Fonts = NewFonts;
	}

	done = true;
	FlushOldAtlas = true;
}


void ImguiOpenGL::FreetypeInit()
{
	if (Freetype.UpdateRebuild())
	{
		//Freetype init
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->TexGlyphPadding = 1;
		unsigned int FontsFlags;
		
		FontsFlags = ImGuiFreeType::ForceAutoHint;
		//FontsFlags |= ImGuiFreeType::Bold;
		
		for (int n = 0; n < io.Fonts->ConfigData.Size; n++)
		{
			auto* font_config = static_cast<ImFontConfig*>(&io.Fonts->ConfigData[n]);
			font_config->RasterizerMultiply = 1.0f;
			font_config->RasterizerFlags = FontsFlags;
		}
		ImGuiFreeType::BuildFontAtlas(io.Fonts, FontsFlags);

		ImGui_ImplOpenGL3_DestroyDeviceObjects();
	    ImGui_ImplOpenGL3_CreateDeviceObjects();
	}
}

GLFWwindow* ImguiOpenGL::GetWindow() const
{
    return window;
}
