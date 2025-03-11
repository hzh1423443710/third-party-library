#include <exception>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// 支持序列化的结构体
struct Message {
	std::string role;
	std::string content;
	// 支持序列化 内置了全局友元函数(to_json, from_json)
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, role, content)
};

/// @brief json 的构造方式
void test_ways_to_json();

/// @brief 解析 json 中的数据
void testParseJson();

/// @brief write json to file
void writeJson();

int main(int argc, char* argv[]) {
	// test_ways_to_json();
	testParseJson();
	// writeJson();

	return 0;
}

inline void test_ways_to_json() {
	// 1.构造 json 对象

	// 1.1 构造 json object
	json j1_1 = {{"name", "json"}};
	json j1_2 = {{"name", "json"}, {"type", "array"}};

	// 1.2 构造 json array
	json j1_3 = {"name", "json", "type", "array"};

	// 1.3 构造 嵌套对象
	// clang-format off
	json j1_4 = {
		{"Compiler", "g++"},
		{"cppStandard", "c++17"},	// 键值对
		{"Server", 					// 嵌套对象
			{
				{"host", "localhost"}, 
				{"port", 8080}
			}
		}
	};
	// clang-format on

	// 2.string -> json
	json j2 = R"(
{
	"Arch": "x86_64",
	"Build": "Debug",
	"Compiler": "g++",
	"cppStandard": "c++17",
	"Server": {
		"host": "localhost",
		"port": 8080
	}
}
)";

	// 3.文件流 -> json (pwd: build)
	std::ifstream f("../.vscode/settings.json");
	if (f) {
		json j3;
		f >> j3; // 3.1 流操作符 会置 file read ptr 为 eof
		// j3 = json::parse(f); // 3.2 parse 失败抛出异常
		std::cout << j3.dump(4) << "\n";
	}

	// 4.支持序列化的结构体
	Message m;
	m.role = "assistant";
	m.content = "hello world!";

	// 4.1 结构体 -> json
	json j = m;
	// 4.2 json -> 结构体
	Message m2 = j.get<Message>(); // Message m2 = j;
	std::cout << m2.role << " : " << m2.content << "\n";

	// 5.构造 json 类型
	json j5 = json::object(); // 空对象: {}
	json j6 = json::array();  // 空数组: []
	j6.push_back(Message{"role", "hello!"});
	j6.push_back(Message{"system", "you are a helper!"});
	std::cout << j6.dump(4) << "\n";
}

inline void testParseJson() {
	// 1.格式错误 中文逗号
	try {
		json j = json::parse(R"( {"uid":1, "message":"你好啊"，"session_id":null})");

	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
	}

	// 2.at, value, operator []
	try {
		json j = {
			{"uid", 1},
			{"message", "hello world"},
			{"session_id", nullptr},
		};
		std::cout << j["status"] << "\n"; // null (operator [] 不存在会创建)
		std::cout << j.dump(4) << "\n";

		std::cout << j.value("not_exist", "default value") << "\n"; // 不存在使用默认值(不创建)
		std::cout << j.dump(4) << "\n";

		std::cout << j.at("not_exist") << "\n"; // at 不存在抛出异常
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
	}
}

inline void writeJson() {
	json data;
	data["status"] = 200;
	data["url"] = "http://localhost:8080";
	data["method"] = "GET";
	data["message"] = "hello world";
	data["isTrue"] = true;
	data["arr"] = {1, 2, 3, 4, 5};
	data["obj"] = {{"name", "json"}, {"type", "object"}};
	data["null"] = nullptr;

	data["emptyArr"] = json::array();
	data["emptyArr"].push_back(1);
	data["emptyArr"].push_back(2);

	data["emptyObj"] = json::object();
	data["emptyObj"]["name"] = "json";

	// 1.not serialize
	std::string buf = data.dump(4);
	std::vector<uint8_t> v(buf.begin(), buf.end());
	// json::parse(v.begin(), v.end());

	// 2.serialize to bson
#if 1
	std::vector<uint8_t> v1 = json::to_bson(data);
#else
	std::vector<uint8_t> v1;
	json::to_bson(data, v1);
#endif
	std::cout << "Binary JSON size: " << v1.size() << ", MongoDB" << "\n";
	// json::from_bson(v1.begin(), v1.end());

	// 3.serialize to ubjson
	std::vector<uint8_t> v3 = json::to_ubjson(data);
	std::cout << "Universal Binary JSON size: " << v3.size() << ", json binary\n";
	// json::from_ubjson(v3.begin(), v3.end());

	// 4.serialize to msgpack
	std::vector<uint8_t> v5 = json::to_msgpack(data);
	std::cout << "MessagePack size: " << v5.size() << ", network\n";
	// json::from_msgpack(v5.begin(), v5.end());

	// 5.ouput to filestream
	std::ofstream out("output.json");
	out << data; // out << data.dump(4);
}