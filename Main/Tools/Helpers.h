#pragma once
#include <string>
#include <chrono>
#include <windows.h>
#include <tlhelp32.h>

std::string BrowseForFolder();

bool DoesPathExist(const std::string& dirPath);
bool DoesFileExist(const std::string& filePath);

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

std::string serializeTimePoint( const std::chrono::system_clock::time_point& time, const std::string& format);

DWORD FindProcessId(char* processName);

BOOL TerminateProcess(DWORD dwProcessId, UINT uExitCode);