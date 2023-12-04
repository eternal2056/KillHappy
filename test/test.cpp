#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread> // �������� sleep ��ͷ�ļ�

#pragma comment(lib, "Ws2_32.lib") // ���ӵ� Winsock ��

int main() {
    // ��ʼ�� Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // �����׽���
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    // ����Ŀ���ַ�Ͷ˿�
    sockaddr_in targetAddr;
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(12345); // ���Ŀ��˿�
    inet_pton(AF_INET, "127.0.0.1", &(targetAddr.sin_addr)); // ���Ŀ�� IP ��ַ

    // ׼��Ҫ���͵�����
    const char* dataToSend = "Hello, UDP World!";

    // ����ѭ����־
    bool shouldRun = true;

    while (shouldRun) {
        // ʹ�� sendto ��������
        int result = sendto(sock, dataToSend, strlen(dataToSend), 0, (const sockaddr*)&targetAddr, sizeof(targetAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Failed to send data: " << WSAGetLastError() << "\n";
        }
        else {
            std::cout << "Data sent successfully\n";
        }

        // ��ͣ 3 ����
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // �ر��׽��ֺ����� Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
