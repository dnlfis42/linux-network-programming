add_executable(tcp_echo_server app/server/main.cpp)
add_executable(tcp_echo_client app/client/main.cpp)

target_include_directories(tcp_echo_server PRIVATE include)
target_include_directories(tcp_echo_client PRIVATE include)
