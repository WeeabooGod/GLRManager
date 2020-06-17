#pragma once
#include "simdjson.h"
#include <string>
#include <vector>

//Basic structure for all games
struct Game
{
	bool operator==(const Game& rhs) const { return this->AppID == rhs.AppID;}
	std::string Name;
	unsigned int AppID{};
};


//Manages the user's config and profiles
class UserProfile
{
	//simdjson stuff
	simdjson::dom::parser GLRMasterListParser;
	simdjson::dom::element jMasterList;

	//For some reason when you load a new file with the same parser, even though we store the element, it gets lost. This parser is for Configs and Profiles instead as we load them on the fly
	simdjson::dom::parser GLRParser;
	simdjson::dom::element jUserConfig;

	//User Paths and Files
	std::string UserAppLocalPath = "";
	std::string UserSteamMasterListPath = "";
	std::string UserConfigPath = "";
	std::string UserProfilePath = "";

	//Default values when no Config exists
	std::string ProgramName = "Greenluma Reborn Manager";
	std::string ProgramVersion = "0.0.5";
	std::string GreenlumaPath = "";
	std::string LastDownloadedList = "";
	std::string LastProfileName = "";

	//Lists of Games on Search
	std::vector<Game> GamesList;

	//Profile Variables
	std::vector<Game> CurrentProfileGames;
	std::string CurrentProfileName;
	std::vector<Game> BlacklistedGames;

	//Profiles list
	std::vector<std::string> ProfileNamesList;


	//Log text list
	std::vector<std::string>LogText;

public:
	UserProfile();
	
	std::string GetJSONFile(const std::string& Path);
	void WriteToConfig();

	//Get Setters for MainProgam
	std::string GetProgramName();
	std::string GetGreenlumaPath();
	void SetGreenlumaPath(std::string Path);
	const std::vector<std::string> GetLogText();

	int GetGameListSize();
	std::string GetGameNameOfIndex(int index);
	std::string GetGameAppIDDOfIndex(int index);
	Game GetGameOfIndex(int index);

	//Profile Member Functions
	void LoadProfile(const std::string& ProfileName);
	void SaveProfile(const std::string& ProfileName);
	void SetProfileGames(std::vector<Game> GameList);
	void SetBlacklistGames(std::vector<Game> GameList);

	void GetProfilesInDirectory();

	//Steam APP List
	void DownloadSteamAPPIDList();
	void SearchListWithKey(const std::string& SearchKey);

};