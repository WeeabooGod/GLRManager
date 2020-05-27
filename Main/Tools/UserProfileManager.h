#pragma once
#include "cJSON.h"
#include <string>

//Manages the user's config and profiles
class UserProfile
{
	cJSON* jConfig = nullptr;
	cJSON* jCurrentProfile = nullptr;

	//User AppDataLocalPath
	std::string UserGLRPath = "";

	//Default values when no Config exists
	std::string DefaultProgramName = "Greenluma Reborn Manager";
	std::string DefaultProgramVersion = "0.0.2";
	std::string DefaultGreenlumaPath = "";

	//User Values
	cJSON* jProgramName;
	cJSON* jProgramVersion;
	cJSON* jGreenlumaPath;

public:
	UserProfile();

	void VerifyConfig();
};