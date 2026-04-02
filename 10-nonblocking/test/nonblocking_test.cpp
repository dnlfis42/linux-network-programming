#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

// -------- 논블로킹 설정 --------

TEST(Nonblocking, SetWithFcntl) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 현재 플래그 확인
    int flags = fcntl(fd, F_GETFL, 0);
    ASSERT_NE(flags, -1);
    std::cout << "before: O_NONBLOCK = " << (flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    EXPECT_EQ(flags & O_NONBLOCK, 0);

    // 논블로킹 설정
    int ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    EXPECT_NE(ret, -1);

    // 확인
    flags = fcntl(fd, F_GETFL, 0);
    std::cout << "after:  O_NONBLOCK = " << (flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    EXPECT_NE(flags & O_NONBLOCK, 0);

    close(fd);
}

TEST(Nonblocking, SetWithSockType) {
    // socket() 생성 시 SOCK_NONBLOCK 플래그로 한번에 설정
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(fd, -1);

    int flags = fcntl(fd, F_GETFL, 0);
    std::cout << "SOCK_NONBLOCK: O_NONBLOCK = "
              << (flags & O_NONBLOCK ? "on" : "off") << "\n";
    EXPECT_NE(flags & O_NONBLOCK, 0);

    close(fd);
}

TEST(Nonblocking, SetWithIoctl) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 현재 상태 확인
    int flags = fcntl(fd, F_GETFL, 0);
    std::cout << "before: O_NONBLOCK = " << (flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    EXPECT_EQ(flags & O_NONBLOCK, 0);

    // ioctl로 논블로킹 설정
    int on = 1;
    int ret = ioctl(fd, FIONBIO, &on);
    EXPECT_NE(ret, -1);

    // 확인
    flags = fcntl(fd, F_GETFL, 0);
    std::cout << "after:  O_NONBLOCK = " << (flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    EXPECT_NE(flags & O_NONBLOCK, 0);

    // 해제
    int off = 0;
    ioctl(fd, FIONBIO, &off);

    flags = fcntl(fd, F_GETFL, 0);
    std::cout << "reset:  O_NONBLOCK = " << (flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    EXPECT_EQ(flags & O_NONBLOCK, 0);

    close(fd);
}

// -------- 논블로킹 accept --------

TEST(Nonblocking, AcceptReturnsEagain) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 논블로킹 설정
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(fd, (sockaddr*)&addr, sizeof(addr));
    listen(fd, 5);

    // 연결 없이 accept — 블록하지 않고 즉시 반환
    int client_fd = accept(fd, nullptr, nullptr);
    EXPECT_EQ(client_fd, -1);
    EXPECT_TRUE(errno == EAGAIN || errno == EWOULDBLOCK);
    std::cout << "accept on nonblocking: " << strerror(errno) << " (" << errno
              << ")\n";

    close(fd);
}

// -------- 논블로킹 recv --------

TEST(Nonblocking, RecvReturnsEagain) {
    // 서버 준비
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    // 클라이언트 연결
    std::thread client([&addr] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(fd, (sockaddr*)&addr, sizeof(addr));
        usleep(200000); // 200ms 대기 — 데이터 안 보냄
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);

    // 논블로킹 설정
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

    // 데이터 없이 recv — 블록하지 않고 즉시 반환
    char buf[64]{};
    ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
    EXPECT_EQ(n, -1);
    EXPECT_TRUE(errno == EAGAIN || errno == EWOULDBLOCK);
    std::cout << "recv on nonblocking (no data): " << strerror(errno) << " ("
              << errno << ")\n";

    close(client_fd);
    close(server_fd);
    client.join();
}

// -------- 논블로킹 connect --------

TEST(Nonblocking, ConnectReturnsEinprogress) {
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(fd, -1);

    // 서버 준비
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    // 논블로킹 connect — 즉시 반환
    int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        EXPECT_EQ(errno, EINPROGRESS);
        std::cout << "nonblocking connect: " << strerror(errno) << " (" << errno
                  << ")\n";
    } else {
        // 로컬이라 즉시 연결될 수도 있음
        std::cout << "nonblocking connect: connected immediately\n";
    }

    // SO_ERROR로 연결 결과 확인
    usleep(10000); // 10ms 대기
    int err = 0;
    len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cout << "SO_ERROR: " << err << " (" << strerror(err) << ")\n";
    EXPECT_EQ(err, 0); // 연결 성공

    close(fd);
    close(server_fd);
}

// -------- 블로킹 vs 논블로킹 비교 --------

TEST(Nonblocking, BlockingVsNonblocking) {
    // 블로킹 소켓
    int blocking_fd = socket(AF_INET, SOCK_STREAM, 0);
    int blocking_flags = fcntl(blocking_fd, F_GETFL, 0);

    // 논블로킹 소켓
    int nonblocking_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int nonblocking_flags = fcntl(nonblocking_fd, F_GETFL, 0);

    std::cout << "blocking    flags: " << blocking_flags
              << " O_NONBLOCK: " << (blocking_flags & O_NONBLOCK ? "on" : "off")
              << "\n";
    std::cout << "nonblocking flags: " << nonblocking_flags << " O_NONBLOCK: "
              << (nonblocking_flags & O_NONBLOCK ? "on" : "off") << "\n";

    EXPECT_EQ(blocking_flags & O_NONBLOCK, 0);
    EXPECT_NE(nonblocking_flags & O_NONBLOCK, 0);

    close(blocking_fd);
    close(nonblocking_fd);
}
