#include "UserProfileManager.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <direct.h>
#include <fstream>


#include "Helpers.h"
#include "curl/curl.h"

UserProfile::UserProfile()
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
	UserSteamMasterListPath = UserAppLocalPath + "MasterAppList/AppListV2.json";


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
	GreenlumaPath = std::string_view(jUserConfig["GreenlumaPath"]).to_string();
	LastDownloadedList = std::string_view(jUserConfig["LastDownloadedList"]).to_string();
	LastProfileName = std::string_view(jUserConfig["LastProfileName"]).to_string();

	
	//First, lets check if the file exist, if it does/does not download the list and put it into our reference cJSON
	if (!DoesFileExist(UserSteamMasterListPath))
	{
		DownloadSteamAPPIDList();
		LogText.emplace_back("> Master SteamAPPIDList Does not Exist! Downloading...");
	}
    else
    {
    	bool Redownloaded = false;
    	//It exist, but is it time to download a new one?
	    std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
	    std::string CurrentTime = serializeTimePoint(input, "%Y-%m-%d %H:%M:%S");
	    std::string PastTime = LastDownloadedList;
	    
	    //Is the current day greater than the past day? (Probably a better way to do this but fuck it)
    	if (PastTime.empty())
    	{
    		//If its empty yet we had the list, redownload
    		DownloadSteamAPPIDList();
    		Redownloaded = true;
    	}
	    else if (CurrentTime.substr(8,2) > PastTime.substr(8, 2))
	    {
	    	//We are a day older, lets refresh the list for safe measure
	        DownloadSteamAPPIDList();
	    	Redownloaded = true;
	    }
	    else if (CurrentTime.substr(8,2) == PastTime.substr(8, 2))
	    {
	        //Might be same day different month
	        if ((CurrentTime.substr(5,2) > PastTime.substr(5, 2)))
	        {
		        DownloadSteamAPPIDList();
	        	Redownloaded = true;
	        }
	        else if (CurrentTime.substr(5,2) == PastTime.substr(5, 2))
	        {
	            //Its same day, same month, is it the same year?
	            if (CurrentTime.substr(0, 4) != PastTime.substr(0, 4))
	            {
	            	//If at this point, its not the same year, then we can also be sure we don't need to redownload list
            		DownloadSteamAPPIDList();
	            	Redownloaded = true;
	            }
	        }
	    }

    	if (Redownloaded)
    	{
    		LogText.emplace_back("> SteamAppIDDlist was a day old! Redownloading...");
    	}
    }
	
	jMasterList = GLRMasterListParser.load(UserSteamMasterListPath);

	//Load the Profiles
	LoadProfile(LastProfileName);	//Load the last profile, defaults to "default" if there was none
	LoadProfile("Blacklist");		//Loads the blacklist file
}

std::string UserProfile::GetJSONFile(const std::string& Path)
{
	//Load File and Parse
    std::ifstream file(Path);
    std::string FileContents;
    FileContents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    
    //Return a raw data
	return FileContents;
}

void UserProfile::WriteToConfig()
{
	//Put all the current vairables into our main buffer, just in case
	auto NewConfig = R"({
	"ProgramName":  ")" + ProgramName + R"(",
	"Version":  ")" + ProgramVersion + R"(",
	"GreenlumaPath":    ")" + GreenlumaPath + R"(",
	"LastDownloadedList":   ")" + LastDownloadedList + R"(",
	"LastProfileName":   ")" + LastProfileName + R"("
})";
	
	//Save what ever we have made modifications to into our json
	std::ofstream ConfigFile(UserConfigPath);
	ConfigFile << NewConfig;
	ConfigFile.close();

	LogText.emplace_back("> Wrote to config.");
}

std::string UserProfile::GetProgramName()
{
    return ProgramName;
}

std::string UserProfile::GetGreenlumaPath()
{
    return GreenlumaPath;
}

void UserProfile::SetGreenlumaPath(std::string Path)
{
	LogText.emplace_back("> Setting Greenluma Path...");
	replace(Path.begin(), Path.end(), '\\', '/');
	GreenlumaPath = Path;
	WriteToConfig();
}

const std::vector<std::string> UserProfile::GetLogText()
{
	return LogText;
}

int UserProfile::GetGameListSize()
{
	return static_cast<int>(GamesList.size());
}

std::string UserProfile::GetGameNameOfIndex(int index)
{
	return GamesList[index].Name;
}

std::string UserProfile::GetGameAppIDDOfIndex(int index)
{
	return std::to_string(GamesList[index].AppID);
}

Game UserProfile::GetGameOfIndex(int index)
{
	return GamesList[index];
}

void UserProfile::LoadProfile(const std::string& ProfileName)
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
			    temp.Name = std::string_view(element["name"]).to_string();
				
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

void UserProfile::SaveProfile(const std::string& ProfileName)
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

void UserProfile::SetProfileGames(std::vector<Game> GameList)
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

void UserProfile::SetBlacklistGames(std::vector<Game> GameList)
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

void UserProfile::DownloadSteamAPPIDList()
{
    std::ofstream CreatedAppListFile(UserSteamMasterListPath);
	
	//Curl the GetAppList from steam, parse it into readbuffer, then put it all into a json file
	std::string databuffer = cURLWebsite("https://api.steampowered.com/ISteamApps/GetAppList/v2/");

	//Create a temporary JSON file with the raw data we just got
	CreatedAppListFile << databuffer;
	CreatedAppListFile.close();

	//Write the time into our config, then call the write to config so our state can be saved even if the program closes prematurely.
	std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
	LastDownloadedList = serializeTimePoint(input, "%Y-%m-%d %H:%M:%S");
	WriteToConfig();

	LogText.emplace_back("> Downloaded SteamAPPIDList v2");
}

void UserProfile::SearchListWithKey(const std::string& SearchKey)
{
	//Clear list just in case
	GamesList.clear();

	std::vector<std::string> SearchWords;
	std::istringstream iss(SearchKey);
	for (std::string s; iss >> s;)
		SearchWords.push_back(s);
	
	//Go through the entire 90000+ json app list. Thankfully simdjson is REALLY fast so this takes naught but a second
    for (simdjson::dom::element element : jMasterList["applist"]["apps"])
	{
	    simdjson::dom::element string = element["name"];
	    std::string AppName = std::string_view(string).to_string();

    	bool Match = true;
    	for (auto word : SearchWords)
    	{
    		//If any word does not match we not good
    		if (AppName.find(word) == std::string::npos)
				Match = false;
    	}
    	
	    if (Match)
	    {
	    	//Some games have pesky "qoutes" in them, honestly they suck.
	    	replace(AppName.begin(), AppName.end(), '"', '-');
	    	
	        Game temp;
	        temp.AppID = static_cast<int>(element["appid"].get<double>());
	        temp.Name = AppName;
	    	
			if (std::find(BlacklistedGames.begin(), BlacklistedGames.end(), temp) == BlacklistedGames.end())
			{
				//The game was not in the blacklist
				GamesList.push_back(temp);
			}
		}
	}
	
	LogText.emplace_back("> Found " + std::to_string(GamesList.size()) + " entries that matched the search.");
	
}
