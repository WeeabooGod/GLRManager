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

	
	//First, lets check if the file exist, if it does/does not download the list and put it into our reference cJSON
	if (!DoesFileExist(UserSteamMasterListPath))
	{
		DownloadSteamAPPIDList();
	}
    else
    {
    	//It exist, but is it time to download a new one?
	    std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
	    std::string CurrentTime = serializeTimePoint(input, "%Y-%m-%d %H:%M:%S");
	    std::string PastTime = LastDownloadedList;
	    
	    //Is the current day greater than the past day? (Probably a better way to do this but fuck it)
    	if (PastTime.empty())
    	{
    		//If its empty yet we had the list, redownload
    		DownloadSteamAPPIDList();
    	}
	    else if (CurrentTime.substr(8,2) > PastTime.substr(8, 2))
	    {
	    	//We are a day older, lets refresh the list for safe measure
	        DownloadSteamAPPIDList();
	    }
	    else if (CurrentTime.substr(8,2) == PastTime.substr(8, 2))
	    {
	        //Might be same day different month
	        if ((CurrentTime.substr(5,2) > PastTime.substr(5, 2)))
	        {
		        DownloadSteamAPPIDList();
	        }
	        else if (CurrentTime.substr(5,2) == PastTime.substr(5, 2))
	        {
	            //Its same day, same month, is it the same year?
	            if (CurrentTime.substr(0, 4) != PastTime.substr(0, 4))
	            {
	            	//If at this point, its not the same year, then we can also be sure we don't need to redownload list
            		DownloadSteamAPPIDList();
	            }
	        }
	    }
    }
	
	jMasterList = GLRParser.load(UserSteamMasterListPath);
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
	"LastDownloadedList":   ")" + LastDownloadedList + R"("
})";
	
	//Save what ever we have made modifications to into our json
	std::ofstream ConfigFile(UserConfigPath);
	ConfigFile << NewConfig;
	ConfigFile.close();
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
	replace(Path.begin(), Path.end(), '\\', '/');
	GreenlumaPath = Path;
	WriteToConfig();
}

int UserProfile::GetGameListSize()
{
	return GamesList.size();
}

std::string UserProfile::GetGameNameOfIndex(int index)
{
	return GamesList[index].Name;
}

std::string UserProfile::GetGameAppIDDOfIndex(int index)
{
	return std::to_string(GamesList[index].AppID);
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
}

void UserProfile::SearchListWithKey(const std::string& SearchKey)
{
	//Clear list just in case
	GamesList.clear();

	//Go through the entire 90000+ json app list. Thankfully simdjson is REALLY fast so this takes naught but a second
    for (simdjson::dom::element element : jMasterList["applist"]["apps"])
    {
        simdjson::dom::element string = element["name"];
        std::string AppName = std::string_view(string).to_string();

        if (AppName.find(SearchKey) != std::string::npos)
        {
	        Game temp;
	        temp.AppID = static_cast<int>(element["appid"].get<double>());
	        temp.Name = AppName;
	        GamesList.push_back(temp);
        }
    }
}
