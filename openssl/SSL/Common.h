#pragma once

#include <iostream>
#include <memory>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// openssl genrsa -out server.key 2048
// openssl req -new -x509 -key server.key -out server.crt -days 365 -subj "/CN=localhost"

// 服务端：SSL certificate file path
constexpr const char* CERT_PATH = "server.crt";
// 服务端：SSL private key file path
constexpr const char* KEY_PATH = "server.key";

// 客户端：CA certificate to verify server (optional)
constexpr const char* CA_CERT_PATH = "server.crt";

using SSL_CTX_ptr = std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)>;
using SSL_ptr = std::unique_ptr<SSL, decltype(&SSL_free)>;

// 创建客户端 SSL_CTX
SSL_CTX_ptr create_ssl_client_ctx() {
	SSL_CTX_ptr ctx(SSL_CTX_new(TLS_client_method()), SSL_CTX_free);
	if (!ctx) {
		std::cerr << "Failed to create SSL context\n";
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	// Set minimum TLS version
	SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);

	// TLS 1.3 cipher suites 密码套件
	SSL_CTX_set_ciphersuites(ctx.get(),
							 "TLS_AES_256_GCM_SHA384:"
							 "TLS_AES_128_GCM_SHA256:"
							 "TLS_CHACHA20_POLY1305_SHA256");

	// TLS 1.2 cipher suites 密码套件
	if (SSL_CTX_set_cipher_list(ctx.get(),
								"ECDHE-ECDSA-AES256-GCM-SHA384:"
								"ECDHE-RSA-AES256-GCM-SHA384:"
								"ECDHE-ECDSA-AES128-GCM-SHA256:"
								"ECDHE-RSA-AES128-GCM-SHA256") != 1) {
		std::cerr << "Failed to set cipher list\n";
		exit(EXIT_FAILURE);
	}

	// certificate verification (optional)
	SSL_CTX_load_verify_locations(ctx.get(), CA_CERT_PATH, nullptr);
	SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, [](int preverify, X509_STORE_CTX* ctx) {
		if (!preverify) {
			int err = X509_STORE_CTX_get_error(ctx);
			int depth = X509_STORE_CTX_get_error_depth(ctx);
			std::cerr << "Certificate verification failed at depth: " << depth
					  << " error: " << X509_verify_cert_error_string(err) << '\n';
		}

		return preverify;
	});

	return ctx;
}

// 创建服务端 SSL_CTX
SSL_CTX_ptr create_ssl_server_ctx() {
	SSL_CTX_ptr ctx(SSL_CTX_new(TLS_server_method()), SSL_CTX_free);
	if (!ctx) {
		std::cerr << "Failed to create SSL_CTX" << '\n';
		exit(EXIT_FAILURE);
	}

	// Set minimum TLS version to 1.2 for better security
	SSL_CTX_set_min_proto_version(ctx.get(), TLS1_2_VERSION);

	// TLS 1.3 cipher suites 密码套件
	SSL_CTX_set_ciphersuites(ctx.get(),
							 "TLS_AES_256_GCM_SHA384:"
							 "TLS_AES_128_GCM_SHA256:"
							 "TLS_CHACHA20_POLY1305_SHA256");

	// TLS 1.2 cipher suites 密码套件
	if (SSL_CTX_set_cipher_list(ctx.get(),
								"ECDHE-ECDSA-AES256-GCM-SHA384:"
								"ECDHE-RSA-AES256-GCM-SHA384:"
								"ECDHE-ECDSA-AES128-GCM-SHA256:"
								"ECDHE-RSA-AES128-GCM-SHA256") != 1) {
		std::cerr << "Failed to set cipher list" << '\n';
		exit(EXIT_FAILURE);
	}

	return ctx;
}

// 配置服务端 SSL_CTX
void configure_server_ctx(SSL_CTX* ctx) {
	// Load certificate
	if (SSL_CTX_use_certificate_file(ctx, CERT_PATH, SSL_FILETYPE_PEM) <= 0) {
		std::cerr << "Failed to load certificate" << '\n';
		exit(EXIT_FAILURE);
	}

	// Load private key
	if (SSL_CTX_use_PrivateKey_file(ctx, KEY_PATH, SSL_FILETYPE_PEM) <= 0) {
		std::cerr << "Failed to load private key" << '\n';
		exit(EXIT_FAILURE);
	}

	// verify private key matches certificate
	if (SSL_CTX_check_private_key(ctx) != 1) {
		std::cerr << "Private key does not match the certificate public key" << '\n';
		exit(EXIT_FAILURE);
	}
}
