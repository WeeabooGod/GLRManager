#pragma once
#include "simdjson.h"
#include <string>
#include <vector>

//Basic structure for all games
struct Game
{
	std::string Name;
	unsigned int AppID{};
};


//Manages the user's config and profiles
class UserProfile
{
	//simdjson stuff
	simdjson::dom::parser GLRParser;
	simdjson::dom::element jMasterList;
	simdjson::dom::element jUserConfig;
	simdjson::dom::element jCurrentGameProfile;

	//User Paths and Files
	std::string UserAppLocalPath = "";
	std::string UserSteamMasterListPath = "";
	std::string UserConfigPath = "";
	std::string UserProfilePath = "";

	//Default values when no Config exists
	std::string ProgramName = "Greenluma Reborn Manager";
	std::string ProgramVersion = "0.0.2";
	std::string GreenlumaPath = "";
	std::string LastDownloadedList = "";

	//Lists of Games on Search
	std::vector<Game> GamesList;

public:
	UserProfile();
	
	std::string GetJSONFile(const std::string& Path);
	void WriteToConfig();

	//Get Setters for MainProgam
	std::string GetProgramName();
	std::string GetGreenlumaPath();
	void SetGreenlumaPath(std::string Path);

	int GetGameListSize();
	std::string GetGameNameOfIndex(int index);
	std::string GetGameAppIDDOfIndex(int index);

	//Steam APP List
	void DownloadSteamAPPIDList();
	void SearchListWithKey(const std::string& SearchKey);

};