#pragma once
//This section deals with the window pop up to direct one to the greenluma path
inline void FindGreenlumaPath(GLRManager& GLRManager)
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
            std::string GLRPath = pathInput;
            GLRPath += "/DLLInjector.exe";

            if (DoesFileExist(GLRPath))
            {
                GLRManager.SetGreenlumaPath(pathInput);
            }
            else
            {
                ImGui::Text("There does not seem to be a DLLInjector.exe in this path.");
            }

            //Only end if there is even a "valid" (not nessesarily correct) path
            if (!GLRManager.GetGreenlumaPath().empty())
            {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}