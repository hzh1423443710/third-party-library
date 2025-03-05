cmake_minimum_required(VERSION 3.20)

project(test_openssl)

find_package(OpenSSL REQUIRED)

add_executable(test_encrypt test_encrypt.cpp)
target_link_libraries(test_encrypt PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# no SSL
add_executable(server_no_ssl No_SSL/tcp_server.cpp)
add_executable(client_no_ssl No_SSL/tcp_client.cpp)

# with SSL
link_libraries(OpenSSL::SSL OpenSSL::Crypto)
add_executable(server_with_ssl SSL/tcp_ssl_server.cpp)
add_executable(client_with_ssl SSL/tcp_ssl_client.cpp)
