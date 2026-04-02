add_executable(tcp_server app/server/main.cpp)
add_executable(tcp_client app/client/main.cpp)

target_include_directories(tcp_server PRIVATE include)
target_include_directories(tcp_client PRIVATE include)

add_subdirectory(test)
