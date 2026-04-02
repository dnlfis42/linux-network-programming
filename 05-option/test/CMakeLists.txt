find_package(GTest REQUIRED)

add_executable(option_test)

target_sources(option_test
    PRIVATE
        option_test.cpp)

target_link_libraries(option_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(option_test)
