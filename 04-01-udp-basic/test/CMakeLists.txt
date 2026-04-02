find_package(GTest REQUIRED)

add_executable(udp_basic_test)

target_sources(udp_basic_test
    PRIVATE
        udp_test.cpp)

target_link_libraries(udp_basic_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(udp_basic_test)
