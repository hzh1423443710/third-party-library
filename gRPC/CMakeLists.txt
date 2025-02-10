cmake_minimum_required(VERSION 3.20)

project(test_grpc)

set(PROTO_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gen)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

# sudo pacman -S grpc protobuf
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

file(GLOB PROTO_GEN_FILES ${PROTO_GEN_DIR}/*.cc)

link_libraries(gRPC::grpc++ protobuf)
include_directories(${PROTO_GEN_DIR})

add_executable(email_verify_rpc_server email_verify_server.cpp ${PROTO_GEN_FILES})
add_executable(email_verify_rpc_client email_verify_client.cpp ${PROTO_GEN_FILES})
