#pragma once
#include "pch.h"

std::vector<std::string> forbidAddressList;
void WriteToLogFile(std::string message)
{
	std::ofstream logfile("D:\\LogFile.txt", std::ios::app); // ���ļ���׷��д��
	if (logfile.is_open())
	{
		logfile << message << std::endl; // д����Ϣ
		logfile.close(); // �ر��ļ�
	}
}

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
void readAndPrintFile(const std::string& filePath) {
	// ���ļ�
	std::ifstream inputFile(filePath);

	// ����ļ��Ƿ�ɹ���
	if (!inputFile.is_open()) {
		readAndPrintFile("Error opening file: ");
		return;
	}

	// ���ж�ȡ�ļ�����
	std::string line;
	while (std::getline(inputFile, line)) {
		// ����ÿһ�е�����
		forbidAddressList.push_back(line);
	}

	// �ر��ļ�
	inputFile.close();
}