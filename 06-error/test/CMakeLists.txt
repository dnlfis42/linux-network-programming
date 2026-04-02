find_package(GTest REQUIRED)

add_executable(error_test)

target_sources(error_test
    PRIVATE
        error_test.cpp)

target_link_libraries(error_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(error_test)
