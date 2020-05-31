//Entrance includes, plus a define for CURL
#include "Startup/SetupImGuiGL.h"
#include "Startup/SetupDockspace.h"


//Pretty much a list of all headers will be in helpers
#include "Tools/Helpers.h"
#include "Tools/UserProfileManager.h"
#include "Tools/FreeTypeFont.h"

#include <thread>
#include <vector>

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
                    GLRProfile.SetGreenlumaPath(pathInput);
                	
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

    	//A list of GameAPPID we would have selected
        std::vector<unsigned int> selectedGameAPPID;
        //A IMGUI Selected List to highlight stuff in Column
        static std::vector<int> selected;

        // render your GUI
    	if (ImGui::Begin("Profiles"))
    	{
    		ImGui::Text("Profiles");
    		ImGui::Button("Hello!");
			ImGui::End();
    	}

        if (ImGui::Begin("Game Search"))
        {
        	ImGui::Spacing();ImGui::Spacing();
        	static char pathInput[1024];
        	ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("GreenLuma Reborn Manager").x / 2));
        	ImGui::Text("GreenLuma Reborn Manager");
        	ImGui::Spacing();
        	
        	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.2f - ImGui::CalcTextSize("Search").x*1.7f);
        	ImGui::InputTextWithHint("", "Search for Game APPID", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None); ImGui::SameLine();
        	
	        if (ImGui::Button("Search"))
	        {
		        std::string SearchKeys = pathInput;

	        	if (!SearchKeys.empty())
	        	{
	        		GLRProfile.SearchListWithKey(SearchKeys);
	        		selectedGameAPPID.clear();
	        		selected.clear();
	        	}
	        }
        	
			ImGui::End();   
        }

    	if (ImGui::Begin("GamesTable"))
    	{
    		//Columnns to show game stuff
        	ImGui::Columns(2, "GameInformation");
        	ImGui::Separator();
        	ImGui::SetColumnWidth(0,ImGui::CalcTextSize("9999999").x*1.5f);
        	ImGui::Text("AppID"); ImGui::NextColumn();
        	ImGui::SetColumnWidth(1,ImGui::GetWindowWidth() - ImGui::CalcTextSize("9999999").x*1.5f);
		    ImGui::Text("Name"); ImGui::NextColumn();
        	ImGui::Separator();

        	//Set all items not selected
        	for (int i = 0; i <  GLRProfile.GetGameListSize(); i++)
        	{
        		selected.push_back(-1);
        	}
        	for (int i = 0; i < GLRProfile.GetGameListSize(); i++)
        	{
        		if (ImGui::Selectable(GLRProfile.GetGameAppIDDOfIndex(i).c_str(), selected[i] == i, ImGuiSelectableFlags_SpanAllColumns))
        		{
        			if (selected[i] == i)
        			{
        				//Deselecting
        				selected[i] = -1;
        				auto iter = std::find(selectedGameAPPID.begin(), selectedGameAPPID.end(), std::stoi(GLRProfile.GetGameAppIDDOfIndex(i)));
        				if (iter != selectedGameAPPID.end())
        				{
        					selectedGameAPPID.erase(iter);
        				}
        			}
                    else
                    {
                    	//selecting
						selected[i] = i;
                    	selectedGameAPPID.push_back(std::stoi(GLRProfile.GetGameAppIDDOfIndex(i)));
                    }
        		}
        		ImGui::NextColumn();
        		ImGui::Text(GLRProfile.GetGameNameOfIndex(i).c_str()); ImGui::NextColumn();
        	}
        	ImGui::Columns(1);
    		ImGui::End();
    	}

        if (ImGui::Begin("TableButtons"))
    	{
    		ImGui::Button("Add Games");ImGui::SameLine((ImGui::GetWindowWidth() - (ImGui::CalcTextSize("BlackList AppID(s)").x * 1.15)));ImGui::Button("BlackList AppID(s)");
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