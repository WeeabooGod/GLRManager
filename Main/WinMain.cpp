//Entrance includes, plus a define for CURL
#include "Startup/SetupImGuiGL.h"
#include "Startup/SetupDockspace.h"


//Other important stuff we need
#include "Tools/Helpers.h"
#include "Tools/GLRManager.h"
#include "Tools/CHBrowserManager.h"

//Final smaller stuff required as well
#include <thread>
#include <vector>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//This will be our browser for use of webscraping
	HeadlessBrowserManager GLRBrowser;
	
	//Create the 
	GLRManager GLRManager;

    //Setup the GL
    ImguiOpenGL ImguiManager(GLRManager.GetProgramName());

	//Vairables used within the loops
    std::vector<Game> SelectedGames; 	  //A list of GameAPPID we would have selected
    static std::vector<int> selected;     //A IMGUI Selected List to highlight stuff in Column
	static int lastSelected = -1;          //Used mostly to allow for shift select

	//Vairables used within the loops (For Profile)
    std::vector<Game> SelectedProfileGames; 	  //A list of GameAPPID we would have selected
    static std::vector<int> selectedProfile;     //A IMGUI Selected List to highlight stuff in Column
	static int lastSelectedProfile = -1;          //Used mostly to allow for shift select

	//Profile Games we have added. If we don't have a profile set to begin with this will just return an empty list anyways;
	std::vector<Game> AddedProfileGames = GLRManager.GetProfileGames();
	static int currentProfileIndex = GLRManager.GetProfileIndexOfNamed(GLRManager.GetCurrentProfileName());
	
	//Bools that force a popup window to come up during search and list as they take the longest
	static bool StartedSearch = false;
	static bool BeginSearch = false;
    static bool BeginNewProfile = false;
	
	//Global Search word, so we can start the search from elsewhere
	std::string SearchWords;
	
    // Main loop
    while (!glfwWindowShouldClose(ImguiManager.GetWindow()))
    {
        //Did we officially open the pop up and begin search? If so, fucking do it then
    	if (BeginSearch == true && StartedSearch == false)
    	{
    		    //Initiate a browser search based on our search keys.
		        GLRBrowser.SearchSteamDB(SearchWords);
	            
		        //Give our list to our GLRManager
		        GLRManager.AppendGameList(GLRBrowser.GetList());
		        
		        //Clear any previous selections
		        SelectedGames.clear();
		        selected.clear();

    			StartedSearch = false;
    			BeginSearch = false;

    			//Close PopUp
    			ImGui::CloseCurrentPopup();
    	}
    	
        // Start the Dear ImGui frame
        ImguiManager.SetupImGuiFrame();
    	
    	//Ask for Path if there is no path
        if (GLRManager.GetGreenlumaPath().empty())
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
                    GLRManager.SetGreenlumaPath(pathInput);
                	
                	//Only end if there is even a "valid" (not nessesarily correct) path
                	if (!GLRManager.GetGreenlumaPath().empty())
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
        //ImGui::ShowDemoWindow();

        // render your GUI
    	if (ImGui::Begin("Profiles"))
    	{
    		ImGui::Text("Profiles");

    		//Index and Lable, if index is -1 we have "none" selected, which also means the number of profiles we have is 0 anyways
    		std::string ComboLable;

            if (currentProfileIndex != -1)
            {
                ComboLable = GLRManager.GetProfileNameOfIndex(currentProfileIndex);
            }
            else
            {
	            ComboLable = "None";
            }
    		
    		if (ImGui::BeginCombo("", ComboLable.c_str(), ImGuiComboFlags_None))
    		{
    			for (int i = 0; i < GLRManager.GetNumberOfProfiles(); i++)
    			{
    				const bool isSelected = (currentProfileIndex == i);
    				if (ImGui::Selectable(GLRManager.GetProfileNameOfIndex(i).c_str(), isSelected))
                        currentProfileIndex = i;

    				//Set the initial focus when opening the combo
    				if (isSelected)
                        ImGui::SetItemDefaultFocus();
    			}
    			
    			ImGui::EndCombo();
    		}
    		
            if (ImGui::Button("New Profile"))
            {
                BeginNewProfile = true;
            }
    		
			ImGui::End();
    	}
    	
    	bool ShiftKeyDownProfile = false;
    	if (ImGui::Begin("ProfilesTable"))
    	{
    		//If Key is Selected
    		if (ImGui::GetIO().KeyShift)
    		{
    			ShiftKeyDownProfile = true;
    		}
            else
            {
	            lastSelectedProfile = -1;
            }

    		static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersHOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg;
    		if (ImGui::BeginTable("##table2", 1, flags))
			{
    			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() - ImGui::GetFontSize() * 7);

				ImGui::TableAutoHeaders();
    			
    			//Data Within Table
    			if (selectedProfile.empty() && !GLRManager.GetProfileGames().empty())
    			{
    				for (int i = 0; i <  GLRManager.GetProfileGames().size(); i++)
		    		{
		    			selectedProfile.push_back(-1);
		    		}
    			}
    			
    			for (int i = 0; i < GLRManager.GetProfileGameListSize(); i++)
    			{
    				ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFontSize() * 1.5f);
    				ImGui::TableSetColumnIndex(0);
		    		if (ImGui::Selectable(GLRManager.ProfileGetGameNameOfIndex(i).c_str(), selectedProfile[i] == i, ImGuiSelectableFlags_SpanAllColumns))
		    		{
		    			ImGui::Spacing();
		    			if (ShiftKeyDownProfile && lastSelectedProfile != -1)
		    			{
		    				int count = 0;
		    				if (lastSelectedProfile > i)
		    				{
		    					count = lastSelectedProfile - i;
		    					for (int j = 0; j < count; j++)
		    					{
		    						if (selectedProfile[i + j] == i + j)
		    						{
		    							//Deselecting
		    							selectedProfile[i + j] = -1;
		    							auto iter = std::find(SelectedProfileGames.begin(), SelectedProfileGames.end(), GLRManager.ProfileGetGameOfIndex(i + j));
		    							if (iter != SelectedProfileGames.end())
		    							{
		    								SelectedProfileGames.erase(iter);
		    							}
		    						}
					                else
					                {
		                				//Selecting
										selectedProfile[i + j] = i + j;
		                				SelectedProfileGames.push_back(GLRManager.ProfileGetGameOfIndex(i + j));
					                }
		    					}
		    				}
                            else
                            {
	                            count = i - lastSelectedProfile;
		    					for (int j = 0; j < count; j++)
		    					{
		    						if (selectedProfile[i - j] == i - j)
		    						{
		    							//Deselecting
		    							selectedProfile[i - j] = -1;
		    							auto iter = std::find(SelectedProfileGames.begin(), SelectedProfileGames.end(), GLRManager.ProfileGetGameOfIndex(i - j));
		    							if (iter != SelectedProfileGames.end())
		    							{
		    								SelectedProfileGames.erase(iter);
		    							}
		    						}
					                else
					                {
		                				//Selecting
										selectedProfile[i - j] = i - j;
		                				SelectedProfileGames.push_back(GLRManager.ProfileGetGameOfIndex(i - j));
					                }
		    					}
                            }
		    			}
                        else
                        {
	                        if (selectedProfile[i] == i)
		    				{
		    					//Deselecting
		    					selectedProfile[i] = -1;
		    					auto iter = std::find(SelectedProfileGames.begin(), SelectedProfileGames.end(), GLRManager.ProfileGetGameOfIndex(i));
		    					if (iter != SelectedProfileGames.end())
		    					{
		    						SelectedProfileGames.erase(iter);
		    					}
		    				}
			                else
			                {
		                		//Selecting
								selectedProfile[i] = i;
		                		SelectedProfileGames.push_back(GLRManager.ProfileGetGameOfIndex(i));
			                }

                        	//If we are trying to multi select, keep track of what we are tryiug to select
                        	if (ShiftKeyDownProfile)
								lastSelectedProfile = i;
                        }
		    		}
    			}
    			ImGui::EndTable();
            }
    		ImGui::End();
		}

    	if (BeginNewProfile)
    	{
    		if (!ImGui::IsPopupOpen("NewProfile"))
    			ImGui::OpenPopup("NewProfile");
            if (ImGui::BeginPopupModal("NewProfile", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Spacing();
            	
                static char pathInput[1024];
            	
                ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x / 3.0f);
                ImGui::InputTextWithHint("", "Profile Name", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None); 
	            ImGui::Spacing();
            	if (ImGui::Button("Confirm"))
            	{
            		std::string text = pathInput;

            		//Only Confirm if we have input
            		if (!text.empty())
            		{
            			//Close Current Popup
            			ImGui::CloseCurrentPopup();

            			//Clear existing list, save new list which auto reloads, along with saving the current selection to our Config
            			GLRManager.ClearProfileGames();
            			GLRManager.SaveProfile(text);
            			GLRManager.GetProfilesInDirectory();
            			GLRManager.WriteToConfig();

            			currentProfileIndex = GLRManager.GetProfileIndexOfNamed(GLRManager.GetCurrentProfileName());

            			//Clear the Selection and Profile Games
            			SelectedProfileGames.clear();
						selectedProfile.clear();
            			
            			BeginNewProfile = false;
            		}
            	}
            	ImGui::SameLine(((ImGui::GetIO().DisplaySize.x / 3.0f) - ImGui::CalcTextSize("Cancel").x * 1.05f));
            	//We are done
                if (ImGui::Button("Cancel"))
                {
					ImGui::CloseCurrentPopup();
                	BeginNewProfile = false;
                }

            	
                ImGui::EndPopup();
            }
    	}

    	if (ImGui::Begin("ProfileTableButtons"))
    	{
    		if (ImGui::Button("Remove Games"))
    		{
    			if (GLRManager.GetProfileGameListSize() != 0)
    			{
    				GLRManager.RemoveProfileGames(SelectedProfileGames);
    				SelectedProfileGames.clear();
    				selectedProfile.clear();
    			}
    		}
    		
			ImGui::End();
    	}
    	
        if (ImGui::Begin("Game Search"))
        {
        	ImGui::Spacing();
        	static char pathInput[1024];
        	ImGui::SameLine((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize("GreenLuma Reborn Manager").x / 2));
        	ImGui::Text("GreenLuma Reborn Manager");
        	ImGui::Spacing();
        	
        	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.2f - ImGui::CalcTextSize("Search").x*1.7f);
        	ImGui::InputTextWithHint("", "Search for Game APPID", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None); ImGui::SameLine();
        	
	        if (ImGui::Button("Search"))
	        {
		        std::string input = pathInput;

	        	if (!input.empty())
	        	{
	        		SearchWords = input;
	        		StartedSearch = true;
	        	}
	        }
			ImGui::End();   
        }

    	//PopUp Part of the Search
    	if (StartedSearch == true)
    	{
    		if (!ImGui::IsPopupOpen("Searching"))
    			ImGui::OpenPopup("Searching");
            if (ImGui::BeginPopupModal("Searching", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
    		{
    			ImGui::Text("Getting games list...");
            	
            	if (BeginSearch == true)
                    StartedSearch = false;
            	BeginSearch = true;
            	
    			ImGui::EndPopup();
    		}
    	}

    	bool ShiftKeyDown = false;
    	if (ImGui::Begin("GamesTable"))
    	{
    		//If Key is Selected
    		if (ImGui::GetIO().KeyShift)
    		{
    			ShiftKeyDown = true;
    		}
            else
            {
	            lastSelected = -1;
            }

    		static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersHOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg;
    		if (ImGui::BeginTable("##table1", 3, flags))
			{
    			ImGui::TableSetupColumn("AppID", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 6);
    			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, ImGui::GetWindowWidth() - ImGui::GetFontSize() * 7);
    			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, (ImGui::GetFontSize() * 7));

				ImGui::TableAutoHeaders();
    			
    			//Data Within Table
    			if (selected.empty() && GLRManager.GetGameListSize() != 0)
    			{
    				for (int i = 0; i <  GLRManager.GetGameListSize(); i++)
		    		{
		    			selected.push_back(-1);
		    		}
    			}
    			
    			for (int i = 0; i < GLRManager.GetGameListSize(); i++)
    			{
    				ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFontSize() * 1.5f);
    				ImGui::TableSetColumnIndex(0);
		    		if (ImGui::Selectable(GLRManager.GetGameAppIDDOfIndex(i).c_str(), selected[i] == i, ImGuiSelectableFlags_SpanAllColumns))
		    		{
		    			ImGui::Spacing();
		    			if (ShiftKeyDown && lastSelected != -1)
		    			{
		    				int count = 0;
		    				if (lastSelected > i)
		    				{
		    					count = lastSelected - i;
		    					for (int j = 0; j < count; j++)
		    					{
		    						if (selected[i + j] == i + j)
		    						{
		    							//Deselecting
		    							selected[i + j] = -1;
		    							auto iter = std::find(SelectedGames.begin(), SelectedGames.end(), GLRManager.GetGameOfIndex(i + j));
		    							if (iter != SelectedGames.end())
		    							{
		    								SelectedGames.erase(iter);
		    							}
		    						}
					                else
					                {
		                				//Selecting
										selected[i + j] = i + j;
		                				SelectedGames.push_back(GLRManager.GetGameOfIndex(i + j));
					                }
		    					}
		    				}
                            else
                            {
	                            count = i - lastSelected;
		    					for (int j = 0; j < count; j++)
		    					{
		    						if (selected[i - j] == i - j)
		    						{
		    							//Deselecting
		    							selected[i - j] = -1;
		    							auto iter = std::find(SelectedGames.begin(), SelectedGames.end(), GLRManager.GetGameOfIndex(i - j));
		    							if (iter != SelectedGames.end())
		    							{
		    								SelectedGames.erase(iter);
		    							}
		    						}
					                else
					                {
		                				//Selecting
										selected[i - j] = i - j;
		                				SelectedGames.push_back(GLRManager.GetGameOfIndex(i - j));
					                }
		    					}
                            }
		    			}
                        else
                        {
	                        if (selected[i] == i)
		    				{
		    					//Deselecting
		    					selected[i] = -1;
		    					auto iter = std::find(SelectedGames.begin(), SelectedGames.end(), GLRManager.GetGameOfIndex(i));
		    					if (iter != SelectedGames.end())
		    					{
		    						SelectedGames.erase(iter);
		    					}
		    				}
			                else
			                {
		                		//Selecting
								selected[i] = i;
		                		SelectedGames.push_back(GLRManager.GetGameOfIndex(i));
			                }

                        	//If we are trying to multi select, keep track of what we are tryiug to select
                        	if (ShiftKeyDown)
								lastSelected = i;
                        }
		    		}
    				
    				ImGui::TableSetColumnIndex(1);
    				ImGui::Text(GLRManager.GetGameNameOfIndex(i).c_str());
    				
    				ImGui::TableSetColumnIndex(2);
    				ImGui::Text(GLRManager.GetGameTypeOfIndex(i).c_str());
    			}

    			
    			ImGui::EndTable();
            }
    		ImGui::End();
		}

        if (ImGui::Begin("TableButtons"))
    	{
    		if (ImGui::Button("Add Games"))
    		{
    			GLRManager.SetProfileGames(SelectedGames);
    			SelectedGames.clear();
    			selected.clear();
    		}
			ImGui::End();
    	}
    	
        //End Dock-space
        EndDockspace();

        //Finish Doing things and render to Screen
        ImguiManager.RenderImGui();
    }

    // Cleanup
    ImguiManager.CleanupImGuiGL();

	//Make sure we properly write all of our vairables back into our config.
    GLRManager.WriteToConfig();
	
    return 0;
}