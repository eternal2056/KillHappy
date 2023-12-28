#pragma once
#include "pch.h"

std::vector<std::string> forbidAddressList;
std::string currentDirectory;
void WriteToLogFile(std::string message)
{
	std::ofstream logfile(currentDirectory + "LogFile.txt", std::ios::app); // 打开文件，追加写入
	//std::ofstream logfile("C:\\D_Files\\Project_Driver\\KillHappy\\x64\\Debug\\LogFile.txt", std::ios::app); // 打开文件，追加写入
	if (logfile.is_open())
	{
		//logfile << message << std::endl; // 写入信息
		logfile.close(); // 关闭文件
	}
}

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
void readAndPrintFile(const std::string& filePath) {
	// 打开文件
	std::ifstream inputFile(filePath);

	// 检查文件是否成功打开
	if (!inputFile.is_open()) {
		readAndPrintFile("Error opening file: ");
		return;
	}

	// 逐行读取文件内容
	std::string line;
	while (std::getline(inputFile, line)) {
		// 处理每一行的数据
		forbidAddressList.push_back(line);
	}

	// 关闭文件
	inputFile.close();
}

std::string getUserPath() {
	char* homePath;
	size_t len;

	// 获取 HOME 环境变量
	if (_dupenv_s(&homePath, &len, "USERPROFILE") == 0 && homePath != nullptr) {
		// 输出当前用户路径
		//std::cout << "当前用户路径：" << homePath << std::endl;
	}
	else {
		//std::cerr << "无法获取当前用户路径。" << std::endl;
	}

	std::string str = homePath;
	free(homePath);
	return str;
}
std::string GetPathWithoutFileName(const std::string& filePath) {
	size_t found = filePath.find_last_of("\\/");
	if (found != std::string::npos) {
		return filePath.substr(0, found);
	}
	return "";
}
std::string getModulePath(HMODULE hModule) {
	// 获取当前模块（DLL）的路径

	char dllPath[MAX_PATH];
	if (GetModuleFileName(hModule, dllPath, MAX_PATH) != 0) {
		// 输出 DLL 路径
	}
	else {
	}
	// 移除文件名
	std::string path =  GetPathWithoutFileName(dllPath) + "\\";
	//WriteToLogFile(path);
	//free(dllPath);
	return path;
}

