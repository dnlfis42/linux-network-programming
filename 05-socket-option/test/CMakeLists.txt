find_package(GTest REQUIRED)

add_executable(socket_option_test)

target_sources(socket_option_test
    PRIVATE
        socket_option_test.cpp)

target_link_libraries(socket_option_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(socket_option_test)
