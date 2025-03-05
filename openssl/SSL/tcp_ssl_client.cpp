#include "Common.h"
#include "../TcpSocket.h"

#include <array>
#include <csignal>

bool running = true;

// Server details
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr uint16_t SERVER_PORT = 5858;

void handle_input(SSL* ssl) {
	std::string input;
	std::array<char, 1024> buffer{};

	while (true) {
		buffer.fill(0);

		// Send data to server
		std::cout << "> ";
		std::getline(std::cin, input);

		if (!running) break;

		if (SSL_write(ssl, input.data(), input.size()) <= 0) {
			std::cerr << "Failed to write to server\n";
			ERR_print_errors_fp(stderr);
			break;
		}

		if (!running) break;

		// Receive response from server
		int bytes = SSL_read(ssl, buffer.data(), buffer.size() - 1);
		if (bytes > 0)
			std::cout << "Server: " << buffer.data() << "\n";
		else {
			std::cerr << "Failed to read from server\n";
			ERR_print_errors_fp(stderr);
			break;
		}
	}
}

int main() {
	// Initialize OpenSSL
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, nullptr);

	// Handle Ctrl+C
	signal(SIGINT, [](int signo) {
		std::cout << "Received signal " << signo << "\n";
		running = false;
	});

	// Create and configure SSL context
	auto ctx = create_ssl_client_ctx();

	// Connect to server
	TcpSocketPtr client = std::make_unique<TcpSocket>();
	client->connect(SERVER_IP, SERVER_PORT);
	std::cout << "Connecting to " << SERVER_IP << ":" << SERVER_PORT << "\n";

	// Create SSL connection
	SSL_ptr ssl(SSL_new(ctx.get()), SSL_free);
	if (!ssl) {
		std::cerr << "Failed to create SSL object\n";
		exit(EXIT_FAILURE);
	}

	// Associate the socket with SSL object
	SSL_set_fd(ssl.get(), client->fd());

	// Perform TLS/SSL handshake
	if (SSL_connect(ssl.get()) != 1) {
		std::cerr << "SSL handshake failed\n";
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	std::cout << "SSL connection established using " << SSL_get_cipher(ssl.get()) << "\n";

	// Optionally verify the server certificate
	if (SSL_get_verify_result(ssl.get()) != X509_V_OK) {
		std::cerr << "Server certificate verification failed\n";
		// In a real application, you might want to abort the connection here
		// Continuing for demonstration purposes
	}

	handle_input(ssl.get());

	// Clean shutdown
	SSL_shutdown(ssl.get());

	std::cout << "Connection closed\n";

	return 0;
}