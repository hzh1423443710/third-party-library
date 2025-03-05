#include "../TcpSocket.h"

#include <array>
#include <thread>
#include <iostream>

#include <csignal>
#include <cstring>

constexpr uint16_t PORT = 5858;

bool running = true;

void handle_client(TcpSocketPtr client) {
	std::array<char, 1024> buffer{};

	while (true) {
		buffer.fill(0);

		// 读取客户端数据
		ssize_t bytes = recv(client->fd(), buffer.data(), buffer.size() - 1, 0);

		if (bytes > 0) {
			std::cout << "Received: " << buffer.data() << '\n';
			send(client->fd(), buffer.data(), bytes, 0);

		} else if (bytes == 0) {
			std::cout << "Connection closed by client" << '\n';
			break;
		} else {
			std::cerr << "Failed to read from client: " << strerror(errno) << '\n';
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	signal(SIGINT, [](int signo) {
		std::cout << "Received signal " << signo << "\n";
		running = false;
	});

	// 创建监听套接字
	TcpSocket listener;

	try {
		// 配置套接字
		listener.setReuseAddr();
		listener.setNonBlocking();
		listener.bind("0.0.0.0", PORT);
		listener.listen();

		std::cout << "Listening on port " << PORT << '\n';

		// 主服务循环
		while (running) {
			// 非阻塞方式接受连接
			TcpSocketPtr client = listener.accept();

			if (!client) {
				// 没有新连接，短暂休眠后继续
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			// 获取并显示客户端信息
			auto [ip, port] = client->peerAddress();
			std::cout << ip << ':' << port << " connected\n";

			// 处理客户端
			handle_client(client);
		}
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "Server shutting down\n";
	return EXIT_SUCCESS;
}