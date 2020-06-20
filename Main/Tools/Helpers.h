#pragma once
#include <string>
#include <chrono>

struct Game
{
	bool operator==(const Game& rhs) const { return this->AppID == rhs.AppID;}
	std::string Name;
	unsigned int AppID{};
};

std::string BrowseForFolder();

bool DoesPathExist(const std::string& dirPath);
bool DoesFileExist(const std::string& filePath);

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::string cURLWebsite(const std::string& URL);

std::string serializeTimePoint( const std::chrono::system_clock::time_point& time, const std::string& format);