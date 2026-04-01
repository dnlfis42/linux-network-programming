#include <sys/socket.h>
#include <unistd.h>

#include <gtest/gtest.h>

TEST(Socket, CreateTcpSocket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);
    close(fd);
}

TEST(Socket, CreateUdpSocket) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(fd, -1);
    close(fd);
}

TEST(Socket, InvalidDomainFails) {
    int fd = socket(-1, SOCK_STREAM, 0);
    EXPECT_EQ(fd, -1);
}
