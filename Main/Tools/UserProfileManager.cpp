#include "UserProfileManager.h"
#include <algorithm>

#include "Helpers.h"
UserProfile::UserProfile()
{
	//Init our profile, therefore we need to get the Local Appdata Location, and if everything is valid, do stuff
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "LOCALAPPDATA") == 0 && buf != nullptr)
    {
        //Assing our Buffer to the string, then free the buffer
        UserGLRPath = buf;
        free(buf);
    }

    //Replace the double backslash with forward slashes and add our program File to the end of it
    replace(UserGLRPath.begin(),UserGLRPath.end(), '\\', '/');
    UserGLRPath += "/GLRAppManager/";

	//Does the path even exist? If it doesn't, create it using _mkdir
	if (!DoesPathExist(UserGLRPath))
	{
        _mkdir(UserGLRPath.c_str());
	}

	//Does Profiles Directory Exist
    if (!DoesPathExist(UserGLRPath + "Profiles/"))
    {
        _mkdir((UserGLRPath + "Profiles/").c_str());
    }

	//Does a Config file exist? If not create it with our default variables
    if (!DoesFileExist(UserGLRPath + "Config.json"))
    {
        std::ofstream CreatedConfigFile(UserGLRPath + "Config.json");

        //Json Object, our "File" so to speak
        cJSON* jTempConfig = cJSON_CreateObject();
    	
        //Program name Variable
        jProgramName = cJSON_CreateString(DefaultProgramName.c_str());
        jProgramVersion = cJSON_CreateString(DefaultProgramVersion.c_str());
        jGreenlumaPath = cJSON_CreateString(DefaultGreenlumaPath.c_str());

        //Add variables into our "file"
        cJSON_AddItemToObject(jTempConfig, "ProgramName", jProgramName);
        cJSON_AddItemToObject(jTempConfig, "Version", jProgramVersion);
        cJSON_AddItemToObject(jTempConfig, "GreenlumaPath", jGreenlumaPath);

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
        std::ifstream file(UserGLRPath + "Config.json");
        std::string FileContents;
        FileContents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        jConfig = cJSON_Parse(FileContents.c_str());

    	//Even if it did exist originally, is it correct and/or up to date?
        VerifyConfig();
    }
}

void UserProfile::VerifyConfig()
{
}
