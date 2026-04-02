find_package(GTest REQUIRED)

add_executable(dns_test)

target_sources(dns_test
    PRIVATE
        dns_test.cpp)

target_link_libraries(dns_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(dns_test)
