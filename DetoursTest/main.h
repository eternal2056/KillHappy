#pragma once

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