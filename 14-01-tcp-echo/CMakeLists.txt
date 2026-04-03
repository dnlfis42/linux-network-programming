add_executable(epoll_echo_server app/server/main.cpp)
add_executable(epoll_echo_client app/client/main.cpp)

target_include_directories(epoll_echo_server PRIVATE include)
target_include_directories(epoll_echo_client PRIVATE include)
