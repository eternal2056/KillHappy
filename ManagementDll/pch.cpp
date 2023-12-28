#pragma once
#include "pch.h"

std::vector<std::string> forbidAddressList;
std::string currentDirectory;
void WriteToLogFile(std::string message)
{
	std::ofstream logfile(currentDirectory + "LogFile.txt", std::ios::app); // ���ļ���׷��д��
	//std::ofstream logfile("C:\\D_Files\\Project_Driver\\KillHappy\\x64\\Debug\\LogFile.txt", std::ios::app); // ���ļ���׷��д��
	if (logfile.is_open())
	{
		//logfile << message << std::endl; // д����Ϣ
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

std::string getUserPath() {
	char* homePath;
	size_t len;

	// ��ȡ HOME ��������
	if (_dupenv_s(&homePath, &len, "USERPROFILE") == 0 && homePath != nullptr) {
		// �����ǰ�û�·��
		//std::cout << "��ǰ�û�·����" << homePath << std::endl;
	}
	else {
		//std::cerr << "�޷���ȡ��ǰ�û�·����" << std::endl;
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
	// ��ȡ��ǰģ�飨DLL����·��

	char dllPath[MAX_PATH];
	if (GetModuleFileName(hModule, dllPath, MAX_PATH) != 0) {
		// ��� DLL ·��
	}
	else {
	}
	// �Ƴ��ļ���
	std::string path =  GetPathWithoutFileName(dllPath) + "\\";
	//WriteToLogFile(path);
	//free(dllPath);
	return path;
}

