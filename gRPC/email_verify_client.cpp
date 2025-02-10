#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::EmailVerifyService;
using message::EmailVerifyRequest;
using message::EmailVerifyResponse;

class EmailVerifyClient {
public:
	explicit EmailVerifyClient(std::shared_ptr<Channel> channel)
		: m_stub(EmailVerifyService::NewStub(channel)) {}

	EmailVerifyResponse getEmailVerifyCode(const std::string& email) {
		EmailVerifyRequest request;
		request.set_email(email);

		ClientContext context;
		EmailVerifyResponse response;

		Status status = m_stub->getEmailVerifyCode(&context, request, &response);
		if (!status.ok()) {
			response.set_error(status.error_message());
		}
		return response;
	}

private:
	// 存根 进行rpc访问
	std::unique_ptr<EmailVerifyService::Stub> m_stub;
};

int main(int argc, char* argv[]) {
	if (argc != 2) return 1;

	try {
		EmailVerifyClient client(
			grpc::CreateChannel("localhost:5001", grpc::InsecureChannelCredentials()));

		EmailVerifyResponse res = client.getEmailVerifyCode(argv[1]);
		std::cout << "status:" << res.status() << "\n";
		std::cout << "error:" << res.error() << "\n";
		std::cout << "code:" << res.code() << "\n";

	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}

	return 0;
}
