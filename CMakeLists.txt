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
add_subdirectory(01-socket)
add_subdirectory(02-address)
add_subdirectory(03-01-tcp-basic)
add_subdirectory(04-01-udp-basic)
add_subdirectory(05-dns)
