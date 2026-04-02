find_package(GTest REQUIRED)

add_executable(tcp_basic_test)

target_sources(tcp_basic_test
    PRIVATE
        tcp_test.cpp)

target_link_libraries(tcp_basic_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tcp_basic_test)
