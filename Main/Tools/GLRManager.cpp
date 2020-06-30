#include "GLRManager.h"

#include <algorithm>
#include <cstdlib>
#include <direct.h>
#include <fstream>
#include <filesystem>
#include <utility>

#include "Helpers.h"

GLRManager::GLRManager()
{
	//Init our profile, therefore we need to get the Local Appdata Location, and if everything is valid, do stuff
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "LOCALAPPDATA") == 0 && buf != nullptr)
    {
        //Assing our Buffer to the string, then free the buffer
        UserAppLocalPath = buf;
        free(buf);
    }

    //Replace the double backslash with forward slashes and add our program File to the end of it
    replace(UserAppLocalPath.begin(), UserAppLocalPath.end(), '\\', '/');

	//Set the paths
	UserAppLocalPath += "/GLRAppManager/";
	UserConfigPath = UserAppLocalPath + "Config.json";
	UserProfilePath = UserAppLocalPath + "Profiles/";

	//Does the path even exist? If it doesn't, create it using _mkdir
	if (!DoesPathExist(UserAppLocalPath))
	{
        _mkdir(UserAppLocalPath.c_str());
	}

	//Does Profiles Directory Exist
    if (!DoesPathExist(UserProfilePath))
    {
        _mkdir((UserProfilePath.c_str()));
    }

	
	//Does a Config file exist? If not create it with our default variables
    if (!DoesFileExist(UserConfigPath))
    {
    	//Default values are already defined, so this will make a file as well as "change" it
    	WriteToConfig();
    }

	//Load our Config
	jUserConfig = GLRParser.load(UserConfigPath);
		
	//We need to get the nessesary information from the Config.
	GreenlumaPath = std::string_view(jUserConfig["GreenlumaPath"]);
	LastProfileName = std::string_view(jUserConfig["LastProfileName"]);

	
	//Load the Profiles
	GetProfilesInDirectory();		// Get all profiles in directory
	LoadProfile(LastProfileName);	// Load the last profile, defaults to "default" if there was none
}

std::string GLRManager::GetJSONFile(const std::string& Path)
{
	//Load File and Parse
    std::ifstream file(Path);
    std::string FileContents;
    FileContents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    
    //Return a raw data
	return FileContents;
}

void GLRManager::WriteToConfig()
{
	//Put all the current vairables into our main buffer, just in case
	auto NewConfig = R"({
	"ProgramName":  ")" + ProgramName + R"(",
	"Version":  ")" + ProgramVersion + R"(",
	"GreenlumaPath":    ")" + GreenlumaPath + R"(",
	"LastProfileName":   ")" + LastProfileName + R"("
})";
	
	//Save what ever we have made modifications to into our json
	std::ofstream ConfigFile(UserConfigPath);
	ConfigFile << NewConfig;
	ConfigFile.close();
}

std::string GLRManager::GetProgramName()
{
    return ProgramName;
}

std::string GLRManager::GetGreenlumaPath()
{
    return GreenlumaPath;
}

void GLRManager::SetGreenlumaPath(std::string Path)
{
	replace(Path.begin(), Path.end(), '\\', '/');
	GreenlumaPath = Path;
	WriteToConfig();
}


int GLRManager::GetGameListSize()
{
	return static_cast<int>(GamesList.size());
}

int GLRManager::GetProfileGameListSize()
{
	return static_cast<int>(CurrentProfileGames.size());
}

std::string GLRManager::GetGameNameOfIndex(int index)
{
	return GamesList[index].Name;
}

std::string GLRManager::GetGameAppIDDOfIndex(int index)
{
	return std::to_string(GamesList[index].AppID);
}

std::string GLRManager::GetGameTypeOfIndex(int index)
{
	return GamesList[index].Type;
}

std::string GLRManager::ProfileGetGameNameOfIndex(int index)
{
	return CurrentProfileGames[index].Name;
}

std::string GLRManager::ProfileGetGameAppIDDOfIndex(int index)
{
	return std::to_string(CurrentProfileGames[index].AppID);
}

std::string GLRManager::ProfileGetGameTypeOfIndex(int index)
{
	return CurrentProfileGames[index].Type;
}

void GLRManager::FilterGames(bool WGames, bool WApplications, bool WMedia, bool WOther)
{
	//Fill the list with the whitelist
	std::vector<std::string> WhiteList;

	int AmountFiltered = 0;
	
	if (WGames)
	{
		WhiteList.emplace_back("Game");
		WhiteList.emplace_back("DLC");
		WhiteList.emplace_back("Demo");
	}

	if (WApplications)
	{
		WhiteList.emplace_back("Application");
		WhiteList.emplace_back("Tool");
		//WhiteList.emplace_back("Application");
		//WhiteList.emplace_back("Application");
	}

	if (WMedia)
	{
		WhiteList.emplace_back("Legacy Media");
		WhiteList.emplace_back("Music");
		WhiteList.emplace_back("Guide");
		WhiteList.emplace_back("Video");
		WhiteList.emplace_back("Series");
	}

	//Go through and see if the words match, if it matches anything its good
	for (int i = 0; i < GamesList.size(); i++)
	{
		bool match = false;
		for (const auto& wType : WhiteList)
		{
			if (wType == GamesList[i].Type)
			{
				match = true;
				break;
			}
		}

		//If we get to this point and nothing matched, but we have other selected, we can safetly assume that anything goes
		if (match == false && WOther)
			match = true;

		//Get rid of the match
		if (!match)
		{
			auto iter = std::find(GamesList.begin(), GamesList.end(), GamesList[i]);
		    if (iter != GamesList.end())
		    {
			    GamesList.erase(iter);
		    	AmountFiltered++;
		    }
		}
		
	}
}

Game GLRManager::GetGameOfIndex(int index)
{
	return GamesList[index];
}

Game GLRManager::ProfileGetGameOfIndex(int index)
{
	return CurrentProfileGames[index];
}

void GLRManager::LoadProfile(const std::string& ProfileName)
{
	CurrentProfileGames.clear();

	if (!ProfileName.empty())
	{
		simdjson::dom::element jTempProfile;
		simdjson::error_code error;
		GLRParser.load(UserProfilePath + ProfileName + ".json").tie(jTempProfile, error);

		if (!error)
		{
			//Go through the entire 90000+ json app list. Thankfully simdjson is REALLY fast so this takes naught but a second
			for (simdjson::dom::element element : jTempProfile["GamesList"])
			{
			    Game temp;
			    temp.AppID = static_cast<int>(element["AppID"].get<double>());
			    temp.Name = std::string_view(element["name"]);
				temp.Type = std::string_view(element["type"]);
				
				CurrentProfileGames.push_back(temp);
			}
			
			CurrentProfileName = ProfileName;
			LastProfileName = ProfileName;
		}
		else
		{
			CurrentProfileName = "";
			LastProfileName = "";
		}
	}
}

void GLRManager::SaveProfile(const std::string& ProfileName)
{
	std::basic_string<char> NewProfile = R"({
)";

	if (!CurrentProfileGames.empty())
	{
		NewProfile += R"(	"GamesList":  [)";

		for (int i = 0; i < CurrentProfileGames.size(); i++)
		{
			NewProfile += R"(
			{ "name": ")" + CurrentProfileGames[i].Name + R"(", "AppID":)" + std::to_string(CurrentProfileGames[i].AppID) + R"(, "type": ")" + CurrentProfileGames[i].Type;

			if (i != CurrentProfileGames.size() - 1)
			{
				NewProfile += R"("},)";
			}
			else
			{
				NewProfile += R"("})";
			}
		}
		NewProfile += R"(
		]
})";
	}
	else
	{
		NewProfile += R"(	"GamesList":  []
)";
		NewProfile += R"(})";
	}

	//Save what ever we have made modifications to into our json
	std::ofstream ProfileFile(UserProfilePath + ProfileName + ".json");
	ProfileFile << NewProfile;
	ProfileFile.close();

	//Reload the Profile
	LoadProfile(ProfileName);
}

void GLRManager::DeleteProfile(const std::string& ProfileName)
{
	//Get all the available profiles that exist and is there
	for (const auto & entry : std::filesystem::directory_iterator(UserProfilePath))
	{
		if (entry.path().filename().extension().generic_string() == ".json")
		{
			//get string then remove the extention
			std::string FileName = entry.path().filename().generic_string();
			FileName = FileName.substr(0, FileName.size() - 5);

			if (FileName == ProfileName)
			{
				std::remove(entry.path().generic_string().c_str());

				CurrentProfileName = "";
				LastProfileName = "";
				return;
			}
		}
	}
}

void GLRManager::SetProfileGames(const std::vector<Game>& GameList)
{
	for (const auto& Game : GameList)
	{
		if (std::find(CurrentProfileGames.begin(), CurrentProfileGames.end(), Game) == CurrentProfileGames.end())
		{
			//The game was not already in the list
			CurrentProfileGames.push_back(Game);
		}
	}

	SaveProfile(CurrentProfileName);
}

void GLRManager::RemoveProfileGames(const std::vector<Game>& GameList)
{
	for (const auto& Game : GameList)
	{
		auto iter = std::find(CurrentProfileGames.begin(), CurrentProfileGames.end(), Game);
		if (iter != CurrentProfileGames.end())
		{
			//The game was not already in the list
			CurrentProfileGames.erase(iter);
		}
	}

	SaveProfile(CurrentProfileName);
}

void GLRManager::ClearProfileGames()
{
	CurrentProfileGames.clear();
}

std::vector<Game> GLRManager::GetProfileGames()
{
	return CurrentProfileGames;
}

std::string GLRManager::GetProfileNameOfIndex(int index)
{
	return ProfileNamesList[index];
}

int GLRManager::GetProfileIndexOfNamed(std::string name)
{
	//Techically if we don't have a current profile set it means we didn't find a "last profile" therefore none
	if (CurrentProfileName.empty())
		return -1;

	auto iter = std::find(ProfileNamesList.begin(), ProfileNamesList.end(), name);
	if (iter != ProfileNamesList.end())
	{
		return static_cast<int>(iter - ProfileNamesList.begin());
	}
	
	return -1;
}

std::string GLRManager::GetCurrentProfileName()
{
	return CurrentProfileName;
}

int GLRManager::GetNumberOfProfiles()
{
	return static_cast<int>(ProfileNamesList.size());
}

void GLRManager::GetProfilesInDirectory()
{
	ProfileNamesList.clear();
	
	//Get all the available profiles that exist and is there
	for (const auto & entry : std::filesystem::directory_iterator(UserProfilePath))
	{
		if (entry.path().filename().extension().generic_string() == ".json")
		{
			//get string then remove the extention
			std::string tempstring = entry.path().filename().generic_string();
			tempstring = tempstring.substr(0, tempstring.size() - 5);

			ProfileNamesList.emplace_back(tempstring);
		}
	}
}

//Function for Sort Below
bool SortByAppIDAcending(const Game& lhs, const Game& rhs)
{
	return lhs.AppID < rhs.AppID;
}

void GLRManager::AppendGameList(std::vector<Game> GeneratedList)
{
	//Clear list we have already.
	GamesList.clear();

	//Copy our GameList with our Generated List
	GamesList = std::move(GeneratedList);
}

void GLRManager::GenerateAppIDList()
{
	//Remove any and all existing files that are within the directory
	for (const auto & entry : std::filesystem::directory_iterator(GreenlumaPath + "/AppList/"))
	{
		std::remove(entry.path().generic_string().c_str());
	}

	//Generate all AppList Files
	for (int i = 0; i < CurrentProfileGames.size(); i++)
	{
		//Save what ever we have made modifications to into our json
		std::ofstream ProfileFile(GreenlumaPath + "/AppList/" + std::to_string(i) + ".txt");
		ProfileFile << CurrentProfileGames[i].AppID;
		ProfileFile.close();
	}
}
