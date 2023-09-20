#include <iostream>
#include <chrono>
#include <thread>

// �������ڼ���Ƿ��Ѿ������ض���ʱ���
bool isTimeReached(std::chrono::system_clock::time_point targetTime) {
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	return currentTime >= targetTime;
}

int main() {
	// ��ȡ��ǰʱ���
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();

	// ����Ŀ��ʱ��㣨��������Ϊ��ǰʱ������5�룩
	std::chrono::seconds timeOffset(5); // ����5���ʱ����
	std::chrono::system_clock::time_point targetTime = currentTime + timeOffset;

	std::cout << "�ȴ�����Ŀ��ʱ��..." << std::endl;

	// ����Ƿ��Ѿ�����Ŀ��ʱ���
	while (!isTimeReached(targetTime)) {
		// �����������������񣬻���ֻ�Ǽ򵥵صȴ�
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ÿ100������һ��
	}

	std::cout << "�Ѿ�����Ŀ��ʱ��㣡" << std::endl;

	return 0;
}
