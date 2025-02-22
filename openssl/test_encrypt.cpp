#include <openssl/sha.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <array>

struct PasswdHasher {
	static std::string passwd_hash(const std::string& password) {
		std::array<uint8_t, SHA256_DIGEST_LENGTH> hash{};

		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, password.c_str(), password.length());
		SHA256_Final(hash.data(), &sha256);

		return to_hex_string(hash);
	}

	static bool passwd_verify(const std::string& passwd, const std::string& hash) {
		return passwd_hash(passwd) == hash;
	}

private:
	static std::string to_hex_string(const std::array<uint8_t, SHA256_DIGEST_LENGTH>& data) {
		std::ostringstream ss;
		for (uint8_t i : data) {
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
		}
		return ss.str();
	}
};

int main(int argc, char* argv[]) {
	std::cout << "Please input first password: ";
	std::string passwd;
	std::cin >> passwd;
	std::string hash1 = PasswdHasher::passwd_hash(passwd);

	while (true) {
		std::cout << "Please input password again: ";
		std::cin >> passwd;
		if (PasswdHasher::passwd_verify(passwd, hash1)) {
			std::cout << "  correct\n";
			break;
		}
		std::cout << "  incorrect\n";
	}

	return 0;
}