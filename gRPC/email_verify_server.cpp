#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::EmailVerifyService;
using message::EmailVerifyRequest;
using message::EmailVerifyResponse;

class EmailVerifyServiceImpl final : public EmailVerifyService::Service {
public:
	Status getEmailVerifyCode(ServerContext* context, const EmailVerifyRequest* request,
							  EmailVerifyResponse* response) override {
		const std::string email = request->email();
		if (email.empty()) {
			response->set_error("Invalid email");
			response->set_status("error");
			return Status::CANCELLED;
		}

		response->set_status("ok");
		response->set_code("653859");

		return Status::OK;
	}
};

int main(int argc, char* argv[]) {
	std::string endpoint("0.0.0.0:5001");
	EmailVerifyServiceImpl email_verify_service;

	ServerBuilder builder;
	builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
	builder.RegisterService(&email_verify_service);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Listening on " << endpoint << "\n";
	server->Wait();

	return 0;
}