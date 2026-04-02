find_package(GTest REQUIRED)

add_executable(nonblocking_test)

target_sources(nonblocking_test
    PRIVATE
        nonblocking_test.cpp)

target_link_libraries(nonblocking_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nonblocking_test)
