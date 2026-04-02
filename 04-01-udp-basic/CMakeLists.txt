add_executable(udp_basic_server app/server/main.cpp)
add_executable(udp_basic_client app/client/main.cpp)

target_include_directories(udp_basic_server PRIVATE include)
target_include_directories(udp_basic_client PRIVATE include)

add_subdirectory(test)
