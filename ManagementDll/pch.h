#pragma once
#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <windows.h>
#include "include\detours.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <winhttp.h>
#include <winternl.h>
#include <cstdio>
#include <TlHelp32.h>

void WriteToLogFile(std::string message);
void readAndPrintFile(const std::string& filePath);
void HookWsaSend();
void HookCreateProcessW();
void HookZwQuerySystemInformation();
void HookRecv();
void FreeWsaSend();
void FreeCreateProcessW();
void FreeZwQuerySystemInformation();
void FreeRecv();
#endif //PCH_H
