#include "ImGuiManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../IMGui/stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* ImGuiOpenGL::GetWindow() const
{
	return window;
}


//Imgui and its libraries has a ton of code to properly control stuff. I COULD reduce the code down to whats needed for me, but this time I decided to keep how ImGui does its demos.
bool ImGuiOpenGL::InitImgui(const std::string& AppName)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return false;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, AppName.c_str(), nullptr, nullptr);
	if (window == nullptr)
		return false;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return false;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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
	io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf", 16.0);

	//Remember Docking Settings
	io.IniFilename = "Resources/DockConfig.ini";

	//Icon for program
	GLFWimage images[1];
	images[0].pixels = stbi_load("Resources/Images/Icon.png", &images[0].width, &images[0].height, nullptr, 4); //rgba channels
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	//We got to the end, therefore our program didn't fuck up.
	return true;
}

void ImGuiOpenGL::SetupImGuiFrame()
{
	//Poll Events
	glfwPollEvents();
	
	//Freetype init
	//FreetypeInit();

	////Once the thread Update Atlas Rebuild we need to flush the old data and create a new one
	//if (FlushOldAtlas == true)
	//{
	//	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	//    ImGui_ImplOpenGL3_CreateDeviceObjects();
	//	FlushOldAtlas = false;
	//}
	
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
}

void ImGuiOpenGL::RenderImGui()
{
    // Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.45f * 1.00f, 0.55f * 1.00f, 0.60f * 1.00f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	glfwSwapBuffers(window);
}

void ImGuiOpenGL::CleanupImGuiGL()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}


//void ImGuiOpenGL::UpdateFontAtlasThread(std::atomic_bool& done)
//{
//	ImGuiIO& io = ImGui::GetIO(); (void)io;
//	
//	ImFontConfig config;
//	config.MergeMode = true;
//	auto* NewFonts = new ImFontAtlas;
//	NewFonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf",16.0);
//	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKjp-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesJapanese());
//	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKkr-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesKorean());
//	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKsc-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
//	NewFonts->AddFontFromFileTTF("Resources/Fonts/NotoSerifCJKtc-Medium.otf",16.0f, &config, io.Fonts->GetGlyphRangesChineseFull());
//	NewFonts->Build();
//
//	//Freetype init
//	NewFonts->TexGlyphPadding = 1;
//	unsigned int FontsFlags;
//	
//	FontsFlags = ImGuiFreeType::ForceAutoHint;
//	//FontsFlags |= ImGuiFreeType::Bold;
//	
//	for (int n = 0; n < NewFonts->ConfigData.Size; n++)
//	{
//		auto* font_config = static_cast<ImFontConfig*>(&NewFonts->ConfigData[n]);
//		font_config->RasterizerMultiply = 1.0f;
//		font_config->RasterizerFlags = FontsFlags;
//	}
//	ImGuiFreeType::BuildFontAtlas(NewFonts, FontsFlags);
//
//	//This is so that if we close prematurely we can use the bool to not try to access the data
//	if (done == false)
//	{
//		io.Fonts = NewFonts;
//	}
//
//	done = true;
//	FlushOldAtlas = true;
//}


//void ImGuiOpenGL::FreetypeInit()
//{
//	if (Freetype.UpdateRebuild())
//	{
//		//Freetype init
//		ImGuiIO& io = ImGui::GetIO();
//		io.Fonts->TexGlyphPadding = 1;
//		unsigned int FontsFlags;
//		
//		FontsFlags = ImGuiFreeType::ForceAutoHint;
//		//FontsFlags |= ImGuiFreeType::Bold;
//		
//		for (int n = 0; n < io.Fonts->ConfigData.Size; n++)
//		{
//			auto* font_config = static_cast<ImFontConfig*>(&io.Fonts->ConfigData[n]);
//			font_config->RasterizerMultiply = 1.0f;
//			font_config->RasterizerFlags = FontsFlags;
//		}
//		ImGuiFreeType::BuildFontAtlas(io.Fonts, FontsFlags);
//
//		ImGui_ImplOpenGL3_DestroyDeviceObjects();
//	    ImGui_ImplOpenGL3_CreateDeviceObjects();
//	}
//}
