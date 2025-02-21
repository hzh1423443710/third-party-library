#include <hiredis/hiredis.h>
#include <iostream>

int main(int argc, char* argv[]) {
	// 连接 redis-server
	redisContext* c = redisConnect("localhost", 6379);
	if (c == nullptr || (c->err != 0)) {
		// 成功 c->err = 0
		if (c != nullptr) {
			std::cerr << "Error: " << c->errstr << '\n';
			redisFree(c);
		} else {
			std::cerr << "Error: Can't allocate redis context" << '\n';
		}
		return 1;
	}

	// SET key value
	auto* reply = (redisReply*)redisCommand(c, "SET %s %s", "foo", "bar");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << '\n';
		redisFree(c);
		return 1;
	}
	if (reply->type == REDIS_REPLY_STATUS) {
		std::cout << "SET: " << reply->str << "\n";
	}
	// 释放 reply 对象
	freeReplyObject(reply);

	// PING
	reply = (redisReply*)redisCommand(c, "PING");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << '\n';
		redisFree(c);
		return 1;
	}
	if (reply->type == REDIS_REPLY_STATUS) {
		std::cout << "PING: " << reply->str << '\n';
	}

	// AUTH password
	reply = (redisReply*)redisCommand(c, "AUTH %s", "1234");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << '\n';
		redisFree(c);
		return 1;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		std::cerr << "AUTH: " << reply->str << "\n";
	}

	// GET key
	reply = (redisReply*)redisCommand(c, "GET %s", "foo");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << '\n';
		redisFree(c);
		return 1;
	}
	std::cout << "GET: " << reply->str << '\n';

	// DEL key
	reply = (redisReply*)redisCommand(c, "DEL %s", "foo");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << '\n';
		redisFree(c);
		return 1;
	}
	if (reply->type == REDIS_REPLY_INTEGER) {
		std::cout << "DEL: " << reply->integer << '\n';
	}

	// 销毁 redisContext
	redisFree(c);
	return 0;
}