#include <exception>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

json g_j = {{"Arch", "x86_64"},
			{"Build", "Debug"},
			{"Compiler", "g++"},
			{"cppStandard", "c++17"},
			{"Server", {{"host", "localhost"}, {"port", 8080}}}};

void readJson() {
	json data;
	// 1.from filestream
	std::ifstream f("compile_commands.json");
	try {
		// f >> data;
		data = json::parse(f);
		std::cout << data.dump(4) << "\n";
		std::cout << data << "\n";
		std::cout << "has Arch field:" << data.contains("Arch") << "\n";

	} catch (std::exception &e) {
		// 解析失败抛出异常
		std::cout << e.what() << "\n";
	}

	// 2.from string
	data = json::parse(g_j.dump());
	std::cout << data["Arch"] << "\n";
	std::cout << data["Server"] << "\n";
	std::cout << data["Server"]["host"].get<std::string>() << "\n";
	std::cout << data["Server"]["host"] << "\n";

	// 2.1查找
	auto it = data.find("Server");
	if (it != data.end()) {
		if (it.value().is_object()) {
			for (auto &[key, value] : it.value().items()) {
				if (value.is_number_unsigned())
					std::cout << key << " : " << value.get<unsigned>() << "\n";
				else
					std::cout << key << " : " << value.get<std::string>() << "\n";
			}
		}
	}

	// 2.2迭代整个json
	for (auto &[key, value] : data.items()) {
		std::cout << key << " : " << value << "\n";
	}
	// for (auto it = data.begin(); it != data.end(); ++it) {
	// 	std::cout << it.key() << " : " << it.value() << "\n";
	// }

	// 3.from string
	data = json::parse(R"(
                        {
                            "happy": true,
                            "pi": 3.141,
                            "host": "localhost",
                            "port": 8080
                        }
                        )");
}

void writeJson() {
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

	// not serialize
	std::string buf = data.dump(4);
	std::vector<uint8_t> v(buf.begin(), buf.end());
// json::parse(v.begin(), v.end());

// serialize to bson
#if 1
	std::vector<uint8_t> v1 = json::to_bson(data);
#else
	std::vector<uint8_t> v1;
	json::to_bson(data, v1);
#endif
	std::cout << "Binary JSON size: " << v1.size() << ", MongoDB" << "\n";
	// json::from_bson(v1.begin(), v1.end());

	// serialize to ubjson
	std::vector<uint8_t> v3 = json::to_ubjson(data);
	std::cout << "Universal Binary JSON size: " << v3.size() << ", json binary\n";
	// json::from_ubjson(v3.begin(), v3.end());

	// serialize to msgpack
	std::vector<uint8_t> v5 = json::to_msgpack(data);
	std::cout << "MessagePack size: " << v5.size() << ", network\n";
	// json::from_msgpack(v5.begin(), v5.end());

	// ouput to filestream
	std::ofstream out("output.json");
	//   out << data;
	out << data.dump(4);
}

int main(int argc, char *argv[]) {
	// readJson();
	writeJson();
	return 0;
}
