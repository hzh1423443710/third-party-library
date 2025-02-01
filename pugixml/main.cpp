#include <fstream>
#include <iostream>

#include <pugixml.hpp>

void test_load_xml() {
	pugi::xml_document doc;

	// 1. 从 xml 文件中加载, doc.load_file(const char *path)
	pugi::xml_parse_result ret = doc.load_file("../pugixml/test.xml");
	if (!ret) {
		std::cerr << ret.description() << "\n";
		return;
	}
	std::cout << ret.description() << "\n";
	if (ret.encoding == pugi::encoding_utf8) std::cout << "encoding: utf8\n";

	// 2. 从 xml 字符串中加载, doc.load_string(const char_t *contents)
	pugi::xml_parse_result ret2 = doc.load_string("");

	// 3.从 buffer 中加载, doc.load_buffer(const void *contents, size_t size)

	// 4.从 std::istream 中加载, doc.load(std::basic_istream<char> &stream)
	std::ifstream input("../pugixml/test.xml");
	pugi::xml_parse_result ret3 = doc.load(input);
}

void test_read_xml() {
	pugi::xml_document doc;

	// 1. 从 xml 文件中加载
	pugi::xml_parse_result ret = doc.load_file("../pugixml/test.xml");
	if (!ret) {
		std::cerr << ret.description() << "\n";
		return;
	}

	// 打印整个xml文档
	doc.print(std::cout, "    ", pugi::format_indent);
	std::cout << "\n";

	// root node
	pugi::xml_node project = doc.child("project");

	// 0 child nodes
	std::cout << "name: " << project.child("name").text().as_string() << "\n";
	std::cout << "version: " << project.child("version").text().as_string() << "\n";
	std::cout << "description: " << project.child("description").text().as_string() << "\n";
	std::cout << "\n";

	// 遍历 environment 节点
	pugi::xml_node env = project.child("environment");
	for (pugi::xml_node node = env.first_child(); node; node = node.next_sibling()) {
		std::cout << node.name() << " : " << node.text().as_string() << "\n";
	}
	std::cout << "\n";

	// 遍历所有 folder 节点
	for (pugi::xml_node folder_node : project.children("folder")) {
		std::cout << folder_node.attribute("name").as_string() << ":\n";
		for (pugi::xml_node file_node : folder_node.children("file")) {
			std::cout << "    " << file_node.text().as_string() << " ("
					  << file_node.attribute("type").as_string() << ")\n";
		}
	}
	std::cout << "\n";

	// 通过 xpath 获取节点
	pugi::xpath_node_set cppfiles = doc.select_nodes("/project/folder/file[@type = '.cpp']");
	for (pugi::xpath_node node : cppfiles) {
		std::cout << node.node().text().as_string() << "\n";
	}
}

void test_write_xml() {
	pugi::xml_document doc;
	// root node
	pugi::xml_node mysql = doc.append_child("mysql");

	// <server> node
	pugi::xml_node server = mysql.append_child("server");
	server.append_child("host").text().set("localhost");
	server.append_child("port").text().set(3306);
	server.append_child("username").text().set("root");
	server.append_child("password").text().set("12345");
	server.append_child("database").text().set("test_db");

	// <options> node
	pugi::xml_node options = mysql.append_child("options");
	options.append_child("auto_commit").text().set("true");
	options.append_child("chatset").text().set("utf8mb4");
	options.append_child("timeout").text().set(30);

	if (!doc.save_file("../pugixml/mysql_config.xml")) {
		std::cerr << "save file failed\n";
		return;
	}
}

int main(int argc, char* argv[]) {
	test_write_xml();
	return 0;
}