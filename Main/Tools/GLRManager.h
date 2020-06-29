#pragma once
#include "../Tools/DataType/GameStruc.h"
#include "simdjson.h"
#include <string>
#include <vector>

//Manages the user's config and profiles
class GLRManager
{
	//simdjson stuff
	simdjson::dom::parser GLRParser;
	simdjson::dom::element jUserConfig;

	//User Paths and Files
	std::string UserAppLocalPath = "";
	std::string UserConfigPath = "";
	std::string UserProfilePath = "";

	//Default values when no Config exists, can be changed anyways
	std::string ProgramName = "Greenluma Reborn Manager";
	std::string ProgramVersion = "0.1.5";
	std::string GreenlumaPath = "";
	std::string LastProfileName = "";

	//Lists of Games on Search
	std::vector<Game> GamesList;

	//Profile Variables
	std::vector<Game> CurrentProfileGames;
	std::string CurrentProfileName;

	//Profiles list
	std::vector<std::string> ProfileNamesList;

	//Log text list
	std::vector<std::string>LogText;

public:
	GLRManager();
	
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
	std::string GetGameTypeOfIndex(int index);
	void FilterGames(bool WGames, bool WApplications, bool WMedia, bool WOther);
	
	Game GetGameOfIndex(int index);

	//Profile Member Functions
	void LoadProfile(const std::string& ProfileName);
	void SaveProfile(const std::string& ProfileName);
	void SetProfileGames(const std::vector<Game>& GameList);

	void GetProfilesInDirectory();
	
	void AppendGameList(std::vector<Game> GeneratedList);

};