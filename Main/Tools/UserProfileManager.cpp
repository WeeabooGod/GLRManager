#include "UserProfileManager.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <direct.h>
#include <fstream>
#include <iomanip>

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

	//Does the path even exist? If it doesn't, create it using _mkdir
	if (!DoesPathExist(UserAppLocalPath))
	{
        _mkdir(UserAppLocalPath.c_str());
	}

	//Does Profiles Directory Exist
    if (!DoesPathExist(UserAppLocalPath + "Profiles/"))
    {
        _mkdir((UserAppLocalPath + "Profiles/").c_str());
    }

	//Does the MasterAppList Directory Exist
	if (!DoesPathExist(UserAppLocalPath + "MasterAppList/"))
	{
		_mkdir(((UserAppLocalPath + "MasterAppList/").c_str()));
	}
	

	//Does a Config file exist? If not create it with our default variables
    if (!DoesFileExist(UserAppLocalPath + "Config.json"))
    {
    	//create file, parse
        std::ofstream CreatedConfigFile(UserAppLocalPath + "Config.json");

        //Json Object, our "File" so to speak
        cJSON* jTempConfig = cJSON_CreateObject();
    	
        //Program name Variable
        jProgramName = cJSON_CreateString(DefaultProgramName.c_str());
        jProgramVersion = cJSON_CreateString(DefaultProgramVersion.c_str());
        jGreenlumaPath = cJSON_CreateString(DefaultGreenlumaPath.c_str());
        jLastDownloadedList = cJSON_CreateString(DefaultLastDownloadedList.c_str());

        //Add variables into our "file"
        cJSON_AddItemToObject(jTempConfig, "ProgramName", jProgramName);
        cJSON_AddItemToObject(jTempConfig, "Version", jProgramVersion);
        cJSON_AddItemToObject(jTempConfig, "GreenlumaPath", jGreenlumaPath);
        cJSON_AddItemToObject(jTempConfig, "LastDownloadedList", jLastDownloadedList);

        //Put raw data into our file now
        CreatedConfigFile << cJSON_Print(jTempConfig);

    	//Copy our jConfigFile to our reference Config File to refer to
        jConfig = jTempConfig;

        //We done here, YEET
        CreatedConfigFile.close();
    }
    else
    {
    	//Load File and Parse
        jConfig = GetJSONFile(UserAppLocalPath + "Config.json");

    	//Even if it did exist originally, is it correct and/or up to date?
        VerifyConfig();
    }

	//Check for the MasterAppList and download it if we need to
	StartupAPPIDList();
}

cJSON* UserProfile::GetJSONFile(const std::string& Path)
{
	//Load File and Parse
    std::ifstream file(Path);
    std::string FileContents;
    FileContents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    
    //Return a parsed CJSON*
    return cJSON_Parse(FileContents.c_str());
}

void UserProfile::VerifyConfig()
{
}

void UserProfile::WriteToConfig()
{
	//Save what ever we have made modifications to into our json
	std::ofstream ConfigFile(UserAppLocalPath + "Config.json");
	ConfigFile << cJSON_Print(jConfig);
	ConfigFile.close();
}

std::string UserProfile::GetProgramName()
{
    return cJSON_GetObjectItem(jConfig, "ProgramName")->valuestring;
}

std::string UserProfile::GetGreenlumaPath()
{
    return cJSON_GetObjectItem(jConfig, "GreenlumaPath")->valuestring;
}

void UserProfile::SetGreenlumaPath(std::string& Path)
{
	replace(Path.begin(), Path.end(), '\\', '/');
	cJSON_ReplaceItemInObject(jConfig, "GreenlumaPath", cJSON_CreateString(Path.c_str()));

	WriteToConfig();
}

void UserProfile::StartupAPPIDList()
{
	//First, lets check if the file exist, if it does/does not download the list and put it into our reference cJSON
	if (!DoesFileExist(UserAppLocalPath + "MasterAppList/AppListV2.json" ))
	{
		jMasterSteamAPPList = DownloadSteamAPPIDList();
	}
    else
    {
    	bool DidWeDownload = false;
    	
	    //It exist, but is it time to download a new one?
    	std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
    	std::string CurrentTime = serializeTimePoint(input, "%Y-%m-%d %H:%M:%S");
    	std::string PastTime = cJSON_GetObjectItem(jConfig, "LastDownloadedList")->valuestring;
    	
    	//Is the current day greater than the past day? (Probably a better way to do this but fuck it)
        if (CurrentTime.substr(8,2) > PastTime.substr(8, 2))
        {
	        jMasterSteamAPPList = DownloadSteamAPPIDList();
        	DidWeDownload = true;
        }
        else if (CurrentTime.substr(8,2) == PastTime.substr(8, 2))
        {
        	//Might be same day different month
	        if ((CurrentTime.substr(5,2) > PastTime.substr(5, 2)))
	        {
		        jMasterSteamAPPList = DownloadSteamAPPIDList();
	        	DidWeDownload = true;
	        }
            else if (CurrentTime.substr(5,2) == PastTime.substr(5, 2))
        	{
            	//Okay same day, same month, is it the same year?
            	if (CurrentTime.substr(0, 4) != PastTime.substr(0, 4))
            	{
            		//If the dates do not equal at this point, just again download the list
            		jMasterSteamAPPList = DownloadSteamAPPIDList();
            		DidWeDownload = true;
            	}
        	}
        }

    	if (DidWeDownload == false)
    	{
    		//Load File and Parse
		    jMasterSteamAPPList = GetJSONFile(UserAppLocalPath + "MasterAppList/AppListV2.json");
    	}
    }
}

cJSON* UserProfile::DownloadSteamAPPIDList()
{
    std::ofstream CreatedAppListFile(UserAppLocalPath + "MasterAppList/AppListV2.json");

	//Curl the GetAppList from steam, parse it into readbuffer, then put it all into a json file
	std::string databuffer = cURLWebsite("https://api.steampowered.com/ISteamApps/GetAppList/v2/");

	//Create a temporary JSON file with the raw data we just got
    cJSON* jTempList = cJSON_Parse(databuffer.c_str());
	CreatedAppListFile << cJSON_Print(jTempList);
	CreatedAppListFile.close();

	//Write the time into our config, then call the write to config so our state can be saved even if the program closes prematurely.
	std::chrono::system_clock::time_point input = std::chrono::system_clock::now();
	cJSON_ReplaceItemInObject(jConfig, "LastDownloadedList", cJSON_CreateString(serializeTimePoint(input, "%Y-%m-%d %H:%M:%S").c_str()));

	WriteToConfig();

	return jTempList;
}