#include <iostream>
#include <memory>
#include <string>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>

using namespace sql;
using StmtPtr = std::unique_ptr<Statement>;
using PstmtPtr = std::unique_ptr<PreparedStatement>;
using ResPtr = std::unique_ptr<ResultSet>;

constexpr const char* DB_NAME = "wechat";

struct User {
	int id{};
	std::string username;
	std::string password;
	std::string email;
	std::string created_at;

	User() = default;
	User(std::string username, std::string password, std::string email)
		: username(std::move(username)), password(std::move(password)), email(std::move(email)) {}

	friend std::ostream& operator<<(std::ostream& os, const User& rhs) {
		os << "id: " << rhs.id << " username: " << rhs.username << " password: " << rhs.password
		   << " email: " << rhs.email << " created_at: " << rhs.created_at;
		return os;
	}
};

int main() {
	try {
		// 获取 MySQL驱动 单例对象
		mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();

		// 创建连接
		Connection* conn = driver->connect("tcp://127.0.0.1:3306", "root", "12345");
		// 设置默认数据库
		conn->setSchema(DB_NAME);

		// DDL: CREATE TABLE
		StmtPtr stmt(conn->createStatement());
		stmt->execute(
			"\
			CREATE TABLE IF NOT EXISTS test_users (\
				id          INT PRIMARY KEY AUTO_INCREMENT,\
				username    VARCHAR(50)     NOT NULL UNIQUE,\
				password    VARCHAR(100)    NOT NULL,\
				email       VARCHAR(100)    NOT NULL UNIQUE,\
				created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP\
			); ");

		// INSERT
		PstmtPtr insert_stmt(
			conn->prepareStatement("\
			INSERT INTO test_users (username,password,email) VALUES (?,?,?);"));

		User user1{"hzh", "12345", "123456@qq.com"};
		insert_stmt->setString(1, user1.username);
		insert_stmt->setString(2, user1.password);
		insert_stmt->setString(3, user1.email);
		insert_stmt->execute();

		User user2{"Mint", "147258369", "142344@qq.com"};
		insert_stmt->setString(1, user2.username);
		insert_stmt->setString(2, user2.password);
		insert_stmt->setString(3, user2.email);
		insert_stmt->execute();

		// Simple SELECT
		ResPtr res(stmt->executeQuery("SELECT * FROM test_users"));
		User user;
		while (res->next()) {
			user.id = res->getInt(1);
			user.username = res->getString(2);
			user.password = res->getString(3);
			user.email = res->getString(4);
			user.created_at = res->getString(5);
			std::cout << user << '\n';
		}

		// UPDATE
		PstmtPtr update_stmt(
			conn->prepareStatement("UPDATE test_users SET password=? WHERE username=?"));
		update_stmt->setString(1, "hzh12345");
		update_stmt->setString(2, "hzh");
		int row = update_stmt->executeUpdate();
		std::cout << "updated row: " << row << '\n';

		// Simple SELECT
		res.reset(stmt->executeQuery("SELECT username,password,email FROM test_users"));
		while (res->next()) {
			std::cout << "username: " << res->getString(1) << " password: " << res->getString(2)
					  << " email: " << res->getString(3) << '\n';
		}

		// DELETE
		PstmtPtr delete_stmt(conn->prepareStatement("DELETE FROM test_users WHERE username=?"));
		delete_stmt->setString(1, "hzh");
		row = delete_stmt->executeUpdate();
		std::cout << "deleted row: " << row << '\n';

	} catch (SQLException& e) {
		std::cerr << "SQLException: " << e.what() << '\n';
		std::cerr << "SQLState: " << e.getSQLState() << '\n';
		std::cerr << "Error Code: " << e.getErrorCode() << '\n';
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << '\n';
	}

	return 0;
}