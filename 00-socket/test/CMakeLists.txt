find_package(GTest REQUIRED)

add_executable(socket_test)

target_sources(socket_test
    PRIVATE
        socket_test.cpp)

target_link_libraries(socket_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(socket_test)
