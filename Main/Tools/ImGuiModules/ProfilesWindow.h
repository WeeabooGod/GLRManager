#pragma once
//This is responsible for creating the Profiles Window on the side
inline void ProfileWindow(GLRManager& GLRManager)
{
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
                //Item is selected
                const bool isSelected = (currentProfileIndex == i);
                if (ImGui::Selectable(GLRManager.GetProfileNameOfIndex(i).c_str(), isSelected))
                {
                    currentProfileIndex = i;
                    GLRManager.LoadProfile(GLRManager.GetProfileNameOfIndex(i));
                    SelectedProfileGames.clear();
                    selectedProfile.clear();
                }

                //Set the initial focus when opening the combo
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        ImGui::Spacing();

        if (ImGui::Button("New Profile"))
        {
            if (!ImGui::IsPopupOpen("NewProfile"))
                ImGui::OpenPopup("NewProfile");
        }
        NewProfilePopUp(GLRManager, SelectedGames, selected, SelectedProfileGames, selectedProfile, currentProfileIndex, AlsoAddGames);

        ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
        ImGui::SameLine();

        ImGui::PushID(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Delete Profile"))
        {
            //Delete Profile
            GLRManager.DeleteProfile(GLRManager.GetProfileNameOfIndex(currentProfileIndex));

            //Clear ProfileGames and Selections
            GLRManager.ClearProfileGames();
            SelectedProfileGames.clear();
            selectedProfile.clear();
            GLRManager.GetProfilesInDirectory();

            //Set Profile index to none as well as the combo lable
            currentProfileIndex = -1;
            ComboLable = "None";
        }
        ImGui::PopStyleColor(1);
        ImGui::PopID();

        //Warning number lable, go above, become red
        int size = GLRManager.GetProfileGameListSize();
        std::string ProfileSizeText = std::to_string(size);
        ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::CalcTextSize("xxxxxx").x + ImGui::CalcTextSize(std::to_string(GLRManager.GetProfileGameListSize()).c_str()).x));
        if (size <= GLRManager.GetAppListLimit())
        {
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.8f, 1), ProfileSizeText.c_str());
        }
        else if (size > GLRManager.GetAppListLimit())
        {
            ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.4f, 1), ProfileSizeText.c_str());
        }
        ImGui::SameLine(); HelpMarker("Size of Profile List. Red means its over limit.");
        ImGui::End();
    }
}