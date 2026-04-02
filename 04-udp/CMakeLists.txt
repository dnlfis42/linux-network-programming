add_executable(udp_server app/server/main.cpp)
add_executable(udp_client app/client/main.cpp)

target_include_directories(udp_server PRIVATE include)
target_include_directories(udp_client PRIVATE include)

add_subdirectory(test)
