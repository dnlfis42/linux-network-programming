#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

// -------- errno / strerror / perror --------

TEST(SocketError, StrerrorReturnsMessage) {
    const char* msg = strerror(ECONNREFUSED);
    EXPECT_NE(msg, nullptr);
    std::cout << "ECONNREFUSED (" << ECONNREFUSED << "): " << msg << "\n";
}

TEST(SocketError, PerrorPrintsMessage) {
    errno = EADDRINUSE;
    testing::internal::CaptureStderr();
    perror("bind");
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_NE(output.find("bind"), std::string::npos);
    std::cout << "perror output: " << output;
    errno = 0;
}

// -------- socket() 에러 --------

TEST(SocketError, InvalidDomain) {
    int fd = socket(-1, SOCK_STREAM, 0);
    EXPECT_EQ(fd, -1);
    std::cout << "socket(-1): " << strerror(errno) << " (" << errno << ")\n";
}

TEST(SocketError, InvalidType) {
    int fd = socket(AF_INET, -1, 0);
    EXPECT_EQ(fd, -1);
    std::cout << "socket(AF_INET, -1): " << strerror(errno) << " (" << errno
              << ")\n";
}

// -------- bind() 에러 --------

TEST(SocketError, BindAddressInUse) {
    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd1, -1);
    ASSERT_NE(fd2, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    ASSERT_EQ(bind(fd1, (sockaddr*)&addr, sizeof(addr)), 0);

    // 커널이 할당한 포트 확인
    socklen_t len = sizeof(addr);
    getsockname(fd1, (sockaddr*)&addr, &len);

    // 같은 포트에 두 번째 바인딩 시도
    int ret = bind(fd2, (sockaddr*)&addr, sizeof(addr));
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(errno, EADDRINUSE);
    std::cout << "EADDRINUSE (" << EADDRINUSE << "): " << strerror(errno)
              << "\n";

    close(fd1);
    close(fd2);
}

// -------- connect() 에러 --------

TEST(SocketError, ConnectRefused) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(59999); // 아무도 안 듣는 포트

    int ret = connect(fd, (sockaddr*)&addr, sizeof(addr));
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(errno, ECONNREFUSED);
    std::cout << "ECONNREFUSED (" << ECONNREFUSED << "): " << strerror(errno)
              << "\n";

    close(fd);
}

// -------- recv() / send() 에러 --------

TEST(SocketError, RecvBadFd) {
    char buf[64]{};
    ssize_t n = recv(-1, buf, sizeof(buf), 0);
    EXPECT_EQ(n, -1);
    EXPECT_EQ(errno, EBADF);
    std::cout << "EBADF (" << EBADF << "): " << strerror(errno) << "\n";
}

TEST(SocketError, SendBadFd) {
    ssize_t n = send(-1, "hello", 5, 0);
    EXPECT_EQ(n, -1);
    EXPECT_EQ(errno, EBADF);
    std::cout << "EBADF (" << EBADF << "): " << strerror(errno) << "\n";
}

TEST(SocketError, SendNotConnected) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    ssize_t n = send(fd, "hello", 5, MSG_NOSIGNAL);
    EXPECT_EQ(n, -1);
    EXPECT_EQ(errno, EPIPE);
    std::cout << "send on unconnected: " << strerror(errno) << " (" << errno
              << ")\n";

    close(fd);
}

// -------- listen() 에러 --------

TEST(SocketError, ListenWithoutBind) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // bind 없이 listen — Linux에서는 성공할 수 있음 (커널이 자동 바인딩)
    int ret = listen(fd, 5);
    std::cout << "listen without bind: ret=" << ret
              << " errno=" << strerror(errno) << " (" << errno << ")\n";

    close(fd);
}

TEST(SocketError, ListenOnUdp) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(fd, -1);

    int ret = listen(fd, 5);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(errno, EOPNOTSUPP);
    std::cout << "listen on UDP: " << strerror(errno) << " (" << errno << ")\n";

    close(fd);
}

// -------- SO_ERROR --------

TEST(SocketError, SoErrorAfterFailedConnect) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(59999);

    connect(fd, (sockaddr*)&addr, sizeof(addr));

    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    std::cout << "SO_ERROR after failed connect: " << strerror(err) << " ("
              << err << ")\n";

    close(fd);
}

// -------- DNS 에러 --------

TEST(SocketError, GaiStrerror) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    int ret =
        getaddrinfo("this.host.does.not.exist.invalid", "80", &hints, &result);
    EXPECT_NE(ret, 0);
    std::cout << "DNS error: " << gai_strerror(ret) << " (" << ret << ")\n";
}
