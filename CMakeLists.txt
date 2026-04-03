cmake_minimum_required(VERSION 3.23)
project(linux-network-programming
    VERSION 1.0.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

enable_testing()

# -------- Sub-projects ---------
add_subdirectory(00-socket)
add_subdirectory(01-address)
add_subdirectory(02-dns)
add_subdirectory(03-tcp)
add_subdirectory(04-udp)
add_subdirectory(05-option)
add_subdirectory(06-error)
add_subdirectory(07-signal)
add_subdirectory(08-tcp-echo)
add_subdirectory(10-nonblocking)
add_subdirectory(14-00-epoll)
add_subdirectory(14-01-tcp-echo)
