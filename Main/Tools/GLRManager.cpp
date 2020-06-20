#include "GLRManager.h"

#include <algorithm>
#include <cstdlib>
#include <direct.h>
#include <fstream>
#include <filesystem>

#include "curl/curl.h"

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

	//Does the MasterAppList Directory Exist
	if (!DoesPathExist(UserAppLocalPath + "MasterAppList/"))
	{
		_mkdir(((UserAppLocalPath + "MasterAppList/").c_str()));
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
	LoadProfile("Blacklist");		// Loads the blacklist file
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

	LogText.emplace_back("> Wrote to config.");
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
	LogText.emplace_back("> Setting Greenluma Path...");
	replace(Path.begin(), Path.end(), '\\', '/');
	GreenlumaPath = Path;
	WriteToConfig();
}

const std::vector<std::string> GLRManager::GetLogText()
{
	return LogText;
}

int GLRManager::GetGameListSize()
{
	return static_cast<int>(GamesList.size());
}

std::string GLRManager::GetGameNameOfIndex(int index)
{
	return GamesList[index].Name;
}

std::string GLRManager::GetGameAppIDDOfIndex(int index)
{
	return std::to_string(GamesList[index].AppID);
}

Game GLRManager::GetGameOfIndex(int index)
{
	return GamesList[index];
}

void GLRManager::LoadProfile(const std::string& ProfileName)
{
	if (ProfileName == "Blacklist")
		BlacklistedGames.clear();
	else
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
				
				if (ProfileName == "Blacklist")
					BlacklistedGames.push_back(temp);
				else
					CurrentProfileGames.push_back(temp);
			}
		}
		
		if (ProfileName != "Blacklist")
			CurrentProfileName = ProfileName;
	}
}

void GLRManager::SaveProfile(const std::string& ProfileName)
{
	std::basic_string<char> NewProfile = R"({
	"GamesList":  [)";

	for (int i = 0; i < BlacklistedGames.size(); i++)
	{
		NewProfile += R"(
		{ "name": ")" + BlacklistedGames[i].Name + R"(", "AppID":)" + std::to_string(BlacklistedGames[i].AppID);

		if (i != BlacklistedGames.size() - 1)
		{
			NewProfile += R"(},)";
		}
		else
		{
			NewProfile += R"(})";
		}
	}
	NewProfile += R"(
	]
})";

	//Save what ever we have made modifications to into our json
	std::ofstream ProfileFile(UserProfilePath + "Blacklist.json");
	ProfileFile << NewProfile;
	ProfileFile.close();

	//Reload the Profile
	LoadProfile(ProfileName);
}			

void GLRManager::SetProfileGames(std::vector<Game> GameList)
{
	for (auto Game : GameList)
	{
		if (std::find(CurrentProfileGames.begin(), CurrentProfileGames.end(), Game) == CurrentProfileGames.end())
		{
			//The game was not already in the list
			CurrentProfileGames.push_back(Game);
		}
	}

	SaveProfile(CurrentProfileName);
}

void GLRManager::SetBlacklistGames(std::vector<Game> GameList)
{
	std::vector<Game> NewList;
	
	for (auto Game : GameList)
	{
		if (std::find(BlacklistedGames.begin(), BlacklistedGames.end(), Game) == BlacklistedGames.end())
		{
			//The game was not already in the list
			BlacklistedGames.push_back(Game);
		}
	}

	//Make a new list that now does not include the blacklisted games
	for (auto List : GamesList)
	{
		if (std::find(BlacklistedGames.begin(), BlacklistedGames.end(), List) == BlacklistedGames.end())
		{
			//The game was not already in the list
			NewList.push_back(List);
		}
	}
	GamesList = NewList;

	SaveProfile("Blacklist");
}

void GLRManager::GetProfilesInDirectory()
{
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

void GLRManager::SearchListWithKey(std::string& SearchKey)
{
	////Clear list just in case
	//GamesList.clear();
	//
	////Create a vector of words in our search
	//std::vector<std::string> SearchWords;
	//std::istringstream iss(SearchKey);
	//for (std::string s; iss >> s;)
	//	SearchWords.push_back(s);

	//////Give SearchWords to load Webpage, then run our Headless Browser.
	//////Create the headless Browser
	////HeadlessBrowserManager SteamSearcher(SearchWords);
	////SteamSearcher.Run();
	//
	//LogText.emplace_back("> Found " + std::to_string(GamesList.size()) + " entries that matched the search.");
	//std::sort(GamesList.begin(), GamesList.end(), SortByAppIDAcending);
}
