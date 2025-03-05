#include "Common.h"
#include "../TcpSocket.h"

#include <array>
#include <thread>

#include <csignal>

constexpr uint16_t PORT = 5858;

bool running = true;

void handle_client(SSL* ssl) {
	std::array<char, 1024> buffer{};

	while (true) {
		buffer.fill(0);

		int bytes = SSL_read(ssl, buffer.data(), buffer.size());

		if (bytes > 0) {
			std::cout << "Received: " << buffer.data() << '\n';
			// echo back
			SSL_write(ssl, buffer.data(), bytes);
		} else {
			int err = SSL_get_error(ssl, bytes);

			if (err == SSL_ERROR_ZERO_RETURN)
				std::cout << "Connection closed by client" << '\n';
			else
				std::cerr << "Failed to read from client" << '\n';

			ERR_print_errors_fp(stderr);
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	// Initialize the SSL library
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);

	signal(SIGINT, [](int signo) {
		std::cout << "Received signal " << signo << "\n";

		running = false;
	});

	// create and configure SSL_CTX
	auto ctx = create_ssl_server_ctx();
	configure_server_ctx(ctx.get());

	TcpSocket listener;
	listener.setReuseAddr();
	listener.setNonBlocking();
	listener.bind("0.0.0.0", PORT);
	listener.listen();
	std::cout << "Listening on port " << PORT << '\n';

	while (running) {
		TcpSocketPtr client = listener.accept();
		if (!client) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		auto [ip, port] = client->peerAddress();
		std::cout << ip << ':' << port << " connected\n";

		// Create new SSL object for the connection
		SSL_ptr ssl(SSL_new(ctx.get()), SSL_free);
		if (!ssl) {
			std::cerr << "Failed to create SSL object" << '\n';
			exit(EXIT_FAILURE);
		}
		SSL_set_fd(ssl.get(), client->fd());

		// SSL handshake
		if (SSL_accept(ssl.get()) <= 0) {
			ERR_print_errors_fp(stderr);
			continue;
		}

		// Handle client
		handle_client(ssl.get());

		// clean shutdown
		SSL_shutdown(ssl.get());
	}

	return 0;
}