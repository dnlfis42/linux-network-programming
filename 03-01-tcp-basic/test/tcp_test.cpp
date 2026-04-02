#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <thread>

#include <gtest/gtest.h>

class TcpTest : public ::testing::Test {
  protected:
    void SetUp() override {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(server_fd, -1);

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        server_addr.sin_port = 0;

        ASSERT_NE(bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)), -1);

        socklen_t len = sizeof(server_addr);
        getsockname(server_fd, (sockaddr*)&server_addr, &len);
    }

    void TearDown() override {
        if (server_fd != -1) close(server_fd);
    }

    int server_fd = -1;
    sockaddr_in server_addr{};
};

TEST_F(TcpTest, BindAndListen) {
    EXPECT_NE(listen(server_fd, 5), -1);
}

TEST_F(TcpTest, ConnectAndAccept) {
    ASSERT_NE(listen(server_fd, 5), -1);

    std::thread client([this] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(fd, -1);
        EXPECT_NE(connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)), -1);
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    EXPECT_NE(client_fd, -1);
    close(client_fd);

    client.join();
}

TEST_F(TcpTest, SendAndRecv) {
    ASSERT_NE(listen(server_fd, 5), -1);

    const char* msg = "hello";

    std::thread client([this, msg] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(fd, -1);
        ASSERT_NE(connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)), -1);
        send(fd, msg, strlen(msg), 0);
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);

    char buf[64]{};
    ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
    EXPECT_GT(n, 0);
    EXPECT_STREQ(buf, "hello");

    close(client_fd);
    client.join();
}
