#include "../TcpSocket.h"

#include <array>
#include <iostream>
#include <string>

#include <csignal>
#include <cstring>

constexpr const char* SERVER_IP = "127.0.0.1";
constexpr uint16_t SERVER_PORT = 5858;

bool running = true;

void handle_input(TcpSocketPtr socket) {
	std::string input;
	std::array<char, 1024> buffer{};

	while (running) {
		buffer.fill(0);

		// 从用户获取输入
		std::cout << "> ";
		std::getline(std::cin, input);

		if (!running) break;

		// 发送数据到服务器
		if (send(socket->fd(), input.data(), input.size(), 0) <= 0) {
			std::cerr << "Failed to write to server: " << strerror(errno) << '\n';
			break;
		}

		if (!running) break;

		// 接收服务器响应
		int bytes = recv(socket->fd(), buffer.data(), buffer.size() - 1, 0);
		if (bytes > 0) {
			std::cout << "Server: " << buffer.data() << "\n";
		} else if (bytes == 0) {
			std::cout << "Server closed connection\n";
			break;
		} else {
			std::cerr << "Failed to read from server: " << strerror(errno) << '\n';
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	// 处理 Ctrl+C 信号
	signal(SIGINT, [](int signo) {
		std::cout << "Received signal " << signo << "\n";
		running = false;
	});

	try {
		// 创建客户端套接字
		TcpSocketPtr client = std::make_unique<TcpSocket>();

		// 连接到服务器
		client->connect(SERVER_IP, SERVER_PORT);

		// 获取连接信息
		auto [server_ip, server_port] = client->peerAddress();
		std::cout << "Connected to " << server_ip << ":" << server_port << "\n";

		// 处理用户输入和服务器通信
		handle_input(std::move(client));

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "Connection closed\n";
	return EXIT_SUCCESS;
}