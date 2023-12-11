#pragma once
#include "pch.h"

std::vector<std::string> forbidAddressList;
void WriteToLogFile(std::string message)
{
	std::ofstream logfile("D:\\LogFile.txt", std::ios::app); // 打开文件，追加写入
	if (logfile.is_open())
	{
		logfile << message << std::endl; // 写入信息
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