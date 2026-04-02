find_package(GTest REQUIRED)

add_executable(socket_error_test)

target_sources(socket_error_test
    PRIVATE
        socket_error_test.cpp)

target_link_libraries(socket_error_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(socket_error_test)
