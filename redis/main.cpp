#include <hiredis/hiredis.h>
#include <iostream>

int main(int argc, char* argv[]) {
	// 连接 redis-server
	redisContext* c = redisConnect("localhost", 6379);
	if (c == nullptr || c->err) {
		if (c) {
			std::cerr << "Error: " << c->errstr << std::endl;
			redisFree(c);
		} else {
			std::cerr << "Error: Can't allocate redis context" << std::endl;
		}
		return 1;
	}

	// SET key value
	redisReply* reply = (redisReply*)redisCommand(c, "SET %s %s", "foo", "bar");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << std::endl;
		redisFree(c);
		return 1;
	}
	std::cout << "SET: " << reply->str << std::endl;
	// 释放 reply 对象
	freeReplyObject(reply);

	// GET key
	reply = (redisReply*)redisCommand(c, "GET %s", "foo");
	if (reply == nullptr) {
		std::cerr << "Error: " << c->errstr << std::endl;
		redisFree(c);
		return 1;
	}
	std::cout << "GET: " << reply->str << std::endl;

	// 销毁 redisContext
	redisFree(c);
	return 0;
}