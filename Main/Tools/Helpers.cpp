#include "Helpers.h"

#include <direct.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include "shlobj.h"   

bool DoesPathExist(const std::string& dirPath)
{
    struct stat buffer {};
    return (stat(dirPath.c_str(), &buffer) == 0);
}

bool DoesFileExist(const std::string& filePath)
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

    if (pidl != nullptr)
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //Set the current directory to path
        SetCurrentDirectory(path);

        //put the path we got into our textbox input

        // free memory used
        IMalloc* imalloc = nullptr;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

    }

    return std::string(path);
}

//Used to get a buffer from a website
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

//Basically make the time into a string
std::string serializeTimePoint( const std::chrono::system_clock::time_point& time, const std::string& format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm = *std::localtime(&tt);
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );
    return ss.str();
}