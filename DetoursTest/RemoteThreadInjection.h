#pragma once
#include "main.h"

std::string getCurrentDirectoryPath();
std::string getUserPath();
BOOL isExistsModules(DWORD processID, std::string moduleName);
std::vector<DWORD> GetChromeProcessIds(const char* processName);
bool IsProcessRunning(DWORD processId);
void InjectionFunction(_TCHAR* argv[]);