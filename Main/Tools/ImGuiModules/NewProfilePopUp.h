#pragma once

//Create a new profile PopUp when we need too
inline static void NewProfilePopUp(GLRManager& GLRManager, std::vector<Game>& SelectedGames, std::vector<int>& selected, std::vector<Game>& SelectedProfileGames, std::vector<int>& selectedProfile, int& currentProfileIndex, bool& AlsoAddGames)
{
    if (ImGui::BeginPopupModal("NewProfile", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Spacing();

        static char pathInput[1024];

        ImGui::SetNextItemWidth(ImGui::GetIO().DisplaySize.x / 3.0f);
        ImGui::InputTextWithHint("", "Profile Name", pathInput, IM_ARRAYSIZE(pathInput), ImGuiInputTextFlags_None);
        ImGui::Separator();
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

                //If the user presssed Add Games to an empty Profile, we need to add those games now for convience sake.
                if (AlsoAddGames == true)
                {
                    GLRManager.SetProfileGames(SelectedGames);
                    SelectedGames.clear();
                    selected.clear();
                    SelectedProfileGames.clear();
                    selectedProfile.clear();
                }

                AlsoAddGames = false;
            }
        }
        ImGui::SameLine(((ImGui::GetIO().DisplaySize.x / 3.0f) - ImGui::CalcTextSize("Cancel").x * 1.05f));
        //We are done
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}