#pragma once
#ifdef NDEBUG
#define IS_SHOW_WINDOW FALSE
#else
#define IS_SHOW_WINDOW TRUE
#endif
#define INITGUID
#include <shobjidl.h> 
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <psapi.h>
#include <chrono>
#include <thread>
#include <filesystem>
#include "InjectDll.h"
#include "AdjustTokenPrivilegesTest.h"
#include "RemoteThreadInjection.h"
#include "AutoStart.h"