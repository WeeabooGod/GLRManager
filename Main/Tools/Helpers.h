#pragma once

#include <string>
#include <algorithm>
#include <direct.h>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <shlobj.h>   

#include "../../Libraries/cJSON/cJSON.h"

std::string InitDirectories();

bool IsPathExist(const std::string& s);
bool IsFileExist(const std::string& filePath);

std::string BrowseForFolder();

void CreateJSON(const std::string& path);
void VerifyJSON(const std::string& path);

void WriteToConfig(cJSON* jConfig, const std::string path);