#pragma once
#include <string>

struct Game
{
	bool operator==(const Game& rhs) const { return this->AppID == rhs.AppID;}
	unsigned int AppID{};
	std::string Name;
	std::string Type;
};
