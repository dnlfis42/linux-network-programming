find_package(GTest REQUIRED)

add_executable(address_test)

target_sources(address_test
    PRIVATE
        address_test.cpp)

target_link_libraries(address_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(address_test)
