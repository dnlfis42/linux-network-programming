#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <thread>

#include <gtest/gtest.h>

TEST(Udp, CreateSocket) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(fd, -1);
    close(fd);
}

TEST(Udp, SendtoAndRecvfrom) {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = 0;

    ASSERT_NE(
        bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)), -1
    );

    socklen_t len = sizeof(server_addr);
    getsockname(server_fd, (sockaddr*)&server_addr, &len);

    const char* msg = "hello udp";

    std::thread client([&server_addr, msg] {
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        ASSERT_NE(fd, -1);
        ssize_t n = sendto(
            fd, msg, strlen(msg), 0, (sockaddr*)&server_addr,
            sizeof(server_addr)
        );
        EXPECT_GT(n, 0);
        close(fd);
    });

    char buf[64]{};
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    ssize_t n = recvfrom(
        server_fd, buf, sizeof(buf), 0, (sockaddr*)&client_addr, &client_len
    );
    EXPECT_GT(n, 0);
    EXPECT_STREQ(buf, "hello udp");

    close(server_fd);
    client.join();
}
