#pragma once
#include "cJSON.h"
#include <string>

//Manages the user's config and profiles
class UserProfile
{
	cJSON* jConfig = nullptr;
	cJSON* jMasterSteamAPPList = nullptr;
	cJSON* jCurrentProfile = nullptr;

	//User AppDataLocalPath
	std::string UserAppLocalPath = "";

	//Default values when no Config exists
	std::string DefaultProgramName = "Greenluma Reborn Manager";
	std::string DefaultProgramVersion = "0.0.2";
	std::string DefaultGreenlumaPath = "";
	std::string DefaultLastDownloadedList = "N/A";

	//User Values
	cJSON* jProgramName;
	cJSON* jProgramVersion;
	cJSON* jGreenlumaPath;
	cJSON* jLastDownloadedList;

public:
	UserProfile();

	
	cJSON* GetJSONFile(const std::string& Path);
	void VerifyConfig();
	void WriteToConfig();


	std::string GetProgramName();
	std::string GetGreenlumaPath();
	void SetGreenlumaPath(std::string& Path);

	void StartupAPPIDList();
	cJSON* DownloadSteamAPPIDList();
};