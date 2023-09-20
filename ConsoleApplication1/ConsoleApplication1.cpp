#include <iostream>
#include <chrono>
#include <thread>

// 函数用于检查是否已经到达特定的时间点
bool isTimeReached(std::chrono::system_clock::time_point targetTime) {
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	return currentTime >= targetTime;
}

int main() {
	// 获取当前时间点
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();

	// 设置目标时间点（这里设置为当前时间点加上5秒）
	std::chrono::seconds timeOffset(5); // 设置5秒的时间间隔
	std::chrono::system_clock::time_point targetTime = currentTime + timeOffset;

	std::cout << "等待到达目标时间..." << std::endl;

	// 检查是否已经到达目标时间点
	while (!isTimeReached(targetTime)) {
		// 这里可以添加其他任务，或者只是简单地等待
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 每100毫秒检查一次
	}

	std::cout << "已经到达目标时间点！" << std::endl;

	return 0;
}
