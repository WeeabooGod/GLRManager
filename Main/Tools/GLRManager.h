#pragma once
#include <string>
#include <vector>
#include "cJSON.h"
#include "../Tools/DataType/GameStruc.h"

//Manages the user's config and profiles
class GLRManager
{
	//User Paths and Files
	std::string UserAppLocalPath = "";
	std::string UserConfigPath = "";
	std::string UserProfilePath = "";

	//Default values when no Config exists, can be changed anyways
	std::string ProgramName = "Greenluma Reborn Manager";
	std::string ProgramVersion = "0.1.5";
	std::string GreenlumaPath = "";
	std::string LastProfileName = "";

	int AppListLimit = 137;

	//Lists of Games on Search
	std::vector<Game> GamesList;

	//Profile Variables
	std::vector<Game> CurrentProfileGames;
	std::string CurrentProfileName;

	//Profiles list
	std::vector<std::string> ProfileNamesList;

public:
	GLRManager();
	
	cJSON* GetJSONFile(const std::string& Path);
	void WriteToConfig();

	//Get Setters for MainProgam
	std::string GetProgramName();
	std::string GetGreenlumaPath();
	void SetGreenlumaPath(std::string Path);

	int GetGameListSize();
	int GetProfileGameListSize();
	
	std::string GetGameNameOfIndex(int index);
	std::string GetGameAppIDDOfIndex(int index);
	std::string GetGameTypeOfIndex(int index);
	std::string ProfileGetGameNameOfIndex(int index);
	std::string ProfileGetGameAppIDDOfIndex(int index);
	std::string ProfileGetGameTypeOfIndex(int index);
	void FilterGames(bool WGames, bool WApplications, bool WMedia, bool WOther);
	
	Game GetGameOfIndex(int index);
	Game ProfileGetGameOfIndex(int index);
	
	void LoadProfile(const std::string& ProfileName);
	void SaveProfile(const std::string& ProfileName);
	void DeleteProfile(const std::string& ProfileName);
	
	void SetProfileGames(const std::vector<Game>& GameList);
	void RemoveProfileGames(const std::vector<Game>& GameList);
	void ClearProfileGames();
	std::vector<Game> GetProfileGames();
	
	std::string GetProfileNameOfIndex(int index);
	int GetProfileIndexOfNamed(std::string name);
	std::string GetCurrentProfileName();
	
	int GetNumberOfProfiles();
	
	void GetProfilesInDirectory();
	
	void AppendGameList(std::vector<Game> GeneratedList);

	void GenerateAppIDList();

	int GetAppListLimit() { return AppListLimit; };

};