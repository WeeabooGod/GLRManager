//Entrance includes, plus a define for CURL
#include "Startup/SetupImGuiGL.h"
#include "Startup/SetupDockspace.h"


//Pretty much a list of all headers will be in helpers
#include "Tools/Helpers.h"
#include "Tools/UserProfileManager.h"
#include "Tools/FreeTypeFont.h"
#include "curl/curl.h"

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Create the programs overall profile
	UserProfile GLRProfile;


    //Setup the GL
    ImguiOpenGL GLRManager(GLRProfile.GetProgramName());

	
    // Main loop
    while (!glfwWindowShouldClose(GLRManager.GetWindow()))
    {
        //OpenGL Shit
        glfwPollEvents();
        glClearColor(0.27f, 0.27f, 0.27f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
    	
        // Start the Dear ImGui frame
        GLRManager.SetupImGuiFrame();

    	//Ask for Path if there is no path
        if (GLRProfile.GetGreenlumaPath().empty())
        {
            ImGui::OpenPopup("Find Greenluma Path");
        	
            if (ImGui::BeginPopupModal("Find Greenluma Path", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("There is no path assosiated for Greenluma, please locate your Greenluma path. (ie. Where your DLLInjector is)");
                ImGui::Separator();
                ImGui::Spacing();
            	
                static char pathInput[1024];
            	
                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.7f);
                ImGui::InputText("", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None); ImGui::SameLine();
            	if (ImGui::Button("Find Path"))
            	{
                    std::string filepath = BrowseForFolder();
                    strcpy_s(pathInput, filepath.c_str());
            	}

            	//We are done
                if (ImGui::Button("Done"))
                {
                	//Set the path we have happened to get. If there wasn't any to begin with then no worries.
                    GLRProfile.SetGreenlumaPath((std::string)pathInput);
                	
                	//Only end if there is even a "valid" (not nessesarily correct) path
                	if (!GLRProfile.GetGreenlumaPath().empty())
                	{
                        ImGui::CloseCurrentPopup();
                	}
                }
            	
                ImGui::EndPopup();
            }
        }
    	
        //Dock-space setup
        SetupDockspace();

        //Show Demo Window
        ImGui::ShowDemoWindow();

        // render your GUI
    	if (ImGui::Begin("Profiles"))
    	{
    		ImGui::Text("Profiles");
    		ImGui::Button("Hello!");
			ImGui::End();
    	}

        if (ImGui::Begin("Game Search"))
        {
        	static char pathInput[1024];

        	ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("GreenLuma Reborn Manager").x / 2));
        	ImGui::Text("GreenLuma Reborn Manager");
        	ImGui::Spacing();
        	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Search").x);
        	ImGui::InputTextWithHint("", "Search for Game APPID", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None); ImGui::SameLine();
	        ImGui::Button("Search");
			ImGui::End();   
        }


        //End Dock-space
        EndDockspace();

        //Finish Doing things and render to Screen
        GLRManager.RenderImGui();
    }

    // Cleanup
    GLRManager.CleanupImGuiGL();

	//Make sure we properly write all of our vairables back into our config.
    GLRProfile.WriteToConfig();
	
    return 0;
}