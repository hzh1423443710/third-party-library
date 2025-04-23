#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <curl/easy.h>

struct MailContent {
	const char* data;
	size_t size;
};

std::string buildMailData(const std::string& from, const std::string& to,
						  const std::string& subject, const std::string& body) {
	std::string mailData = "From: " + from + "\r\n";
	mailData += "To: " + to + "\r\n";
	mailData += "Subject: " + subject + "\r\n\r\n";
	mailData += body + "\r\n";

	return mailData;
}

/**
 * @brief 自定义读回调函数 (size*nitems标识一个数据块大小)
 * @param buffer 缓冲区, 大小 size * nitems
 * @param size 每个元素的大小  默认为1
 * @param nitems 元素的数量   默认为65536(64KB)
 * @param userdata 用户数据
 * @return 实际读取的字节数, 0代表传输完成
 */
static size_t read_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
	MailContent* mail = static_cast<MailContent*>(userdata);
	size_t buffer_size = size * nitems;

	std::cout << "read_callback: " << mail->size << ", " << size << "x" << nitems << "="
			  << buffer_size << "\n";

	if (mail->size == 0) {
		return 0; // 没有更多数据可读
	}

	size_t copy_size = (mail->size < buffer_size) ? mail->size : buffer_size;
	memcpy(buffer, mail->data, copy_size);
	mail->data += copy_size;
	mail->size -= copy_size;
	return copy_size;
}

int main(int argc, char* argv[]) {
	std::cout << "libcurl version: " << curl_version() << "\n";
	CURL* curl = curl_easy_init();

	if (curl == nullptr) {
		std::cerr << "Failed to initialize curl\n";
		return 1;
	}

	// SMTP服务器信息
	std::string smtp_server = "smtps://smtp.126.com:587";
	std::string username = "h1423443710@126.com";
	std::string password = "RKjDMqAnaAQfEa7k";

	// 发件人和收件人
	std::string from = "h1423443710@126.com";
	std::string to = "1423443710@qq.com";
	std::string mail_data =
		buildMailData(from, to, "Test Email", "This is a test email sent using libcurl.");
	std::cout << mail_data;

	MailContent mail_ctx{};
	mail_ctx.data = mail_data.c_str();
	mail_ctx.size = mail_data.size();

	// 收件人列表
	struct curl_slist* recipients = nullptr;
	recipients = curl_slist_append(recipients, to.c_str());

	// 配置 curl 选项
	curl_easy_setopt(curl, CURLOPT_URL, smtp_server.c_str());
	curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
	curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL); // 使用SSL/TLS

	curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
	curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

	curl_easy_setopt(curl, CURLOPT_READDATA, &mail_ctx); // 回调期间确保数据不被释放
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // 打印详细信息

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
	} else {
		std::cout << "Email sent successfully!\n";
	}

	curl_slist_free_all(recipients);
	curl_easy_cleanup(curl);

	return 0;
}