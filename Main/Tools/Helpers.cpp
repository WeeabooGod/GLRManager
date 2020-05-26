#include "Helpers.h"

//Default variables mostly in one place, used ONLY for Config creation
#include "../Config/ConfigVariables.h"

bool IsPathExist(const std::string& s)
{
    struct stat buffer{};
    return (stat(s.c_str(), &buffer) == 0);
}

std::string InitDirectories()
{
    //Path to our AppData
    std::string AppDataPath;

    //Get LocalAppData Location and if everything is valid, do stuff
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "LOCALAPPDATA") == 0 && buf != nullptr)
    {
        //Assing our Buffer to the string, then free the buffer
        AppDataPath = buf;
        free(buf);
    }

    //Replace the double backslash with forward slashes and add our program File to the end of it
    replace(AppDataPath.begin(), AppDataPath.end(), '\\', '/');
    AppDataPath += "/GLRAppManager/";

    //Does the path even exist?
    if (!IsPathExist(AppDataPath))
    {
        //It Doesn't exist so lets create the directory
        _mkdir(AppDataPath.c_str());
    }

    //Does the Profile path even exist?
    if (!IsPathExist(AppDataPath + "Profiles/"))
    {
        _mkdir((AppDataPath + "Profiles/").c_str());
    }

    //Do we even have a Config.json to use?
    if (!IsFileExist((AppDataPath + "Config.json")))
    {
        //It doesn't exist
        CreateJSON(AppDataPath + "Config.json");
    }

	//Verify the Json, because if it did exist originally, does it have the proper information?
    VerifyJSON(AppDataPath + "Config.json");
	
	//return the path
    return AppDataPath;
}

bool IsFileExist(const std::string& filePath)
{
	const std::ifstream file(filePath);
    return file.good();
}

std::string BrowseForFolder()
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = ("All Folders Automatically Recursed.");
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //Set the current directory to path
        SetCurrentDirectory(path);

        //put the path we got into our textbox input

        // free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

    }

    return std::string(path);
}

void CreateJSON(const std::string& path)
{
    std::ofstream ConfigFile(path);
	
    //Json Object, our "File" so to speak
    cJSON* jConfigFile = cJSON_CreateObject();
	
	//Program name Variable
    cJSON* jProgramName = cJSON_CreateString(DefaultProgramName.c_str());
    cJSON* jProgramVersion = cJSON_CreateString(VersionNumber.c_str());
    cJSON* jGreenlumaPath = cJSON_CreateString(GreenlumaPath.c_str());

	
    //Add variables into our "file"
    cJSON_AddItemToObject(jConfigFile, "ProgramName", jProgramName);
    cJSON_AddItemToObject(jConfigFile, "Version", jProgramVersion);
    cJSON_AddItemToObject(jConfigFile, "GreenlumaPath", jGreenlumaPath);
	
	//Put raw data into our file now
    ConfigFile << cJSON_Print(jConfigFile);

	//We done here, YEET
    ConfigFile.close();
}

void VerifyJSON(const std::string& path)
{
}

void WriteToConfig(cJSON* jConfig, const std::string path)
{
    std::ofstream ConfigFile(path);

	ConfigFile << cJSON_Print(jConfig);
}


