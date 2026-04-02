find_package(GTest REQUIRED)

add_executable(signal_test)

target_sources(signal_test
    PRIVATE
        signal_test.cpp)

target_link_libraries(signal_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(signal_test)
