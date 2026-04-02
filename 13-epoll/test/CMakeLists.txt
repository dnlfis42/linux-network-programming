find_package(GTest REQUIRED)

add_executable(epoll_test)

target_sources(epoll_test
    PRIVATE
        epoll_test.cpp)

target_link_libraries(epoll_test
    PRIVATE
        GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(epoll_test)
