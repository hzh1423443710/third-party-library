#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/kazuho-picojson/defaults.h>
#include <jwt-cpp/traits/kazuho-picojson/traits.h>
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <thread>

class JWTUtil {
public:
	JWTUtil(std::string secret) : m_secret(std::move(secret)) {}

	// 生成 JWT Token
	std::string generateToken(const std::string& subject,
							  const std::map<std::string, std::string>& claims = {},
							  const std::chrono::seconds& valid_for = std::chrono::hours{
								  24}) const {
		try {
			auto now = std::chrono::system_clock::now();
			auto token = jwt::create()
							 .set_issuer(m_issuer)
							 .set_type("JWT")
							 .set_issued_at(now)
							 .set_expires_at(now + valid_for)
							 .set_subject(subject);

			// payload claims
			for (const auto& claim : claims) {
				token.set_payload_claim(claim.first, jwt::claim(claim.second));
			}

			// HS256 算法签名
			return token.sign(jwt::algorithm::hs256{m_secret});
		} catch (const std::exception& e) {
			throw std::runtime_error("Error generating token: " + std::string(e.what()));
		}
	}

	// 验证 JWT Token 返回解码后的 JWT
	jwt::decoded_jwt<jwt::traits::kazuho_picojson> verifyToken(const std::string& token) const {
		try {
			auto decoded = jwt::decode(token);

			// 验证器
			auto verifer = jwt::verify()
							   .allow_algorithm(jwt::algorithm::hs256{m_secret})
							   .with_issuer(m_issuer)
							   .leeway(60); // 允许60s时间偏差

			verifer.verify(decoded);
			return decoded;
		} catch (const std::exception& e) {
			throw std::runtime_error("Error verifying token: " + std::string(e.what()));
		}
	}

	static std::string getSubject(const std::string& token) {
		try {
			auto decoded = jwt::decode(token);
			return decoded.get_subject();
		} catch (const std::exception& e) {
			throw std::runtime_error("Error getting subject: " + std::string(e.what()));
		}
	}

	static std::string getClaim(const std::string& token, const std::string& claim_name) {
		try {
			auto decoded = jwt::decode(token);
			return decoded.get_payload_claim(claim_name).as_string();
		} catch (const std::exception& e) {
			throw std::runtime_error("Error getting claim: " + std::string(e.what()));
		}
	}

	static bool isTokenExpired(const std::string& token) {
		try {
			auto decoded = jwt::decode(token);
			auto exp = decoded.get_expires_at();
			auto now = std::chrono::system_clock::now();
			return exp < now;
		} catch (const std::exception& e) {
			throw std::runtime_error("Error checking token expiration: " + std::string(e.what()));
		}
	}

	void setIssuer(const std::string& issuer) noexcept { m_issuer = issuer; }

	std::string getIssuer() const noexcept { return m_issuer; }

private:
	std::string m_secret;
	std::string m_issuer = "auth0";
};

void test1() noexcept {
	try {
		JWTUtil jwt_util("my_secret_key");

		// 自定义声明
		std::map<std::string, std::string> claims = {
			{"role", "admin"}, {"name", "hzh"}, {"email", "123456@qq.com"}};

		// 生成 Token
		std::string token = jwt_util.generateToken("user123", claims, std::chrono::hours{24});
		std::cout << "Generated Token: " << token << "\n";

		// 验证 Token
		auto decoded = jwt_util.verifyToken(token);
		std::cout << decoded.get_header() << "\n";
		std::cout << decoded.get_payload() << "\n";
		std::cout << "Decoded Token Success" << "\n";

		// 获取信息
		std::cout << "Subject: " << JWTUtil::getSubject(token) << "\n";
		std::cout << "Role: " << JWTUtil::getClaim(token, "role") << "\n";
		std::cout << "Name: " << JWTUtil::getClaim(token, "name") << "\n";
		std::cout << "Email: " << JWTUtil::getClaim(token, "email") << "\n";
		std::cout << "Is Token Exp: " << JWTUtil::isTokenExpired(token) << "\n";

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	} catch (...) {
		std::cerr << "Unknown error occurred.\n";
	}
}

void test2() noexcept {
	try {
		JWTUtil jwt_util("my_secret_key");

		// 自定义声明
		std::map<std::string, std::string> claims = {
			{"role", "admin"}, {"name", "hzh"}, {"email", "123456@qq.com"}};

		// 生成 Token
		std::string token = jwt_util.generateToken("user123", claims, std::chrono::hours{24});
		std::cout << "Generated Token: " << token << "\n";

		// 修改 token 使其无效
		token[0] = '1';

		// 验证 Token
		auto decoded = jwt_util.verifyToken(token);
		// std::cout << decoded.get_token() << "\n";
		std::cout << decoded.get_header() << "\n";
		std::cout << decoded.get_payload() << "\n";
		std::cout << "Decoded Token Success" << "\n";

		// 获取信息
		std::cout << "Subject: " << JWTUtil::getSubject(token) << "\n";
		std::cout << "Role: " << JWTUtil::getClaim(token, "role") << "\n";
		std::cout << "Name: " << JWTUtil::getClaim(token, "name") << "\n";
		std::cout << "Email: " << JWTUtil::getClaim(token, "email") << "\n";
		std::cout << "Is Token Exp: " << JWTUtil::isTokenExpired(token) << "\n";

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	} catch (...) {
		std::cerr << "Unknown error occurred.\n";
	}
}

void test3() noexcept {
	try {
		JWTUtil jwt_util("my_secret_key");

		// 自定义声明
		std::map<std::string, std::string> claims = {
			{"role", "admin"}, {"name", "hzh"}, {"email", "123456@qq.com"}};

		// 生成 Token
		std::string token = jwt_util.generateToken("user123", claims, std::chrono::seconds{10});
		std::cout << "Generated Token: " << token << "\n";

		// 等待 10 秒钟
		std::this_thread::sleep_for(std::chrono::seconds{11});

		// 验证 Token
		auto decoded = jwt_util.verifyToken(token);
		// std::cout << decoded.get_token() << "\n";
		std::cout << decoded.get_header() << "\n";
		std::cout << decoded.get_payload() << "\n";
		std::cout << "Decoded Token Success" << "\n";

		// 获取信息
		std::cout << "Subject: " << JWTUtil::getSubject(token) << "\n";
		std::cout << "Role: " << JWTUtil::getClaim(token, "role") << "\n";
		std::cout << "Name: " << JWTUtil::getClaim(token, "name") << "\n";
		std::cout << "Email: " << JWTUtil::getClaim(token, "email") << "\n";
		std::cout << "Is Token Exp: " << JWTUtil::isTokenExpired(token) << "\n";

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	} catch (...) {
		std::cerr << "Unknown error occurred.\n";
	}
}

int main(int argc, char* argv[]) {
	test1();
	std::cout << "\n";
	test2();
	std::cout << "\n";
	test3();

	return 0;
}