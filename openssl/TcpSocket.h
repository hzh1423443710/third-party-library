#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstdint>
#include <string>
#include <memory>
#include <stdexcept>

class TcpSocket;
using TcpSocketPtr = std::shared_ptr<TcpSocket>;

class TcpSocket {
public:
	TcpSocket(const TcpSocket&) = delete;
	TcpSocket(TcpSocket&&) = delete;
	TcpSocket& operator=(const TcpSocket&) = delete;
	TcpSocket& operator=(TcpSocket&&) = delete;

	explicit TcpSocket() : m_fd(socket(AF_INET, SOCK_STREAM, 0)) {
		if (m_fd < 0) {
			throw std::runtime_error("Failed to create socket");
		}
	}

	explicit TcpSocket(int fd) : m_fd(fd) {
		if (m_fd < 0) {
			throw std::runtime_error("Failed to create socket");
		}
	}

	~TcpSocket() { close(m_fd); }

	int fd() const { return m_fd; }

	// 设置非阻塞
	void setNonBlocking(bool non_blocking = true) {
		int flags = fcntl(m_fd, F_GETFL, 0);
		if (flags < 0) {
			throw std::runtime_error("Failed to get socket flags");
		}

		flags = non_blocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);

		if (fcntl(m_fd, F_SETFL, flags) < 0) {
			throw std::runtime_error("Failed to set socket flags");
		}
	}

	bool isNonBlocking() const {
		int flags = fcntl(m_fd, F_GETFL, 0);
		if (flags < 0) {
			throw std::runtime_error("Failed to get socket flags");
		}

		return (flags & O_NONBLOCK) != 0;
	}

	// 设置端口复用
	void setReuseAddr(bool reuse = true) {
		int option = reuse ? 1 : 0;
		if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
			throw std::runtime_error("Failed to set socket reuse address option");
		}
	}

	bool isReuseAddr() const {
		bool option;
		socklen_t len = sizeof(option);
		if (getsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &option, &len) < 0) {
			throw std::runtime_error("Failed to get socket reuse address option");
		}

		return option;
	}

	// bind
	void bind(const std::string& ip, uint16_t port) {
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
			throw std::runtime_error("Invalid address or address not supported");
		}

		if (::bind(m_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
			throw std::runtime_error("Failed to bind socket");
		}
	}

	// listen
	void listen(int backlog = 10) {
		if (::listen(m_fd, backlog) < 0) {
			throw std::runtime_error("Failed to listen on socket");
		}
	}

	// accept
	TcpSocketPtr accept() {
		sockaddr_in addr{};
		socklen_t len = sizeof(addr);

		int client_fd = ::accept(m_fd, reinterpret_cast<sockaddr*>(&addr), &len);
		if (client_fd < 0) {
			// 非阻塞
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
				return nullptr;
			}
			throw std::runtime_error("Failed to accept connection");
		}

		return std::make_unique<TcpSocket>(client_fd);
	}

	// connect
	bool connect(const std::string& ip, uint16_t port) {
		sockaddr_in server_addr{};
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);

		if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
			throw std::runtime_error("Invalid address or address not supported");
		}

		if (::connect(m_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
			throw std::runtime_error("Connection failed");
		}

		return true;
	}

	// local address
	std::pair<std::string, uint16_t> localAddress() const {
		sockaddr_in addr{};
		socklen_t len = sizeof(addr);

		if (getsockname(m_fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0) {
			throw std::runtime_error("Failed to get local address");
		}

		return {inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)};
	}

	// peer address
	std::pair<std::string, uint16_t> peerAddress() const {
		sockaddr_in addr{};
		socklen_t len = sizeof(addr);

		if (getpeername(m_fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0) {
			throw std::runtime_error("Failed to get peer address");
		}

		return {inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)};
	}

private:
	int m_fd;
};
