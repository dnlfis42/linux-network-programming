#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include <gtest/gtest.h>

// -------- Reuse --------

TEST(SocketOptionReuse, ReuseAddr) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, &len);
    std::cout << "SO_REUSEADDR default: " << val << "\n";
    EXPECT_EQ(val, 0);

    int on = 1;
    std::cout << "SO_REUSEADDR set    : " << on << "\n";
    EXPECT_EQ(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)), 0);

    getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, &len);
    std::cout << "SO_REUSEADDR after  : " << val << "\n";
    EXPECT_EQ(val, 1);

    close(fd);
}

TEST(SocketOptionReuse, ReusePort) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, &len);
    std::cout << "SO_REUSEPORT default: " << val << "\n";
    EXPECT_EQ(val, 0);

    int on = 1;
    std::cout << "SO_REUSEPORT set    : " << on << "\n";
    EXPECT_EQ(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)), 0);

    getsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, &len);
    std::cout << "SO_REUSEPORT after  : " << val << "\n";
    EXPECT_EQ(val, 1);

    close(fd);
}

// -------- Buffer --------

TEST(SocketOptionBuffer, RcvBuf) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, &len);
    int default_val = val;
    std::cout << "SO_RCVBUF default: " << default_val << "\n";
    EXPECT_GT(default_val, 0);

    int new_val = 65536;
    std::cout << "SO_RCVBUF set    : " << new_val << "\n";
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &new_val, sizeof(new_val));

    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, &len);
    std::cout << "SO_RCVBUF after  : " << val << "\n";
    EXPECT_GE(val, new_val);

    close(fd);
}

TEST(SocketOptionBuffer, ZeroRcvBuf) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, &len);
    std::cout << "SO_RCVBUF default: " << val << "\n";
    EXPECT_GT(val, 0);

    int new_val = 0;
    std::cout << "SO_RCVBUF set    : " << new_val << "\n";
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &new_val, sizeof(new_val));

    getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, &len);
    std::cout << "SO_RCVBUF after  : " << val << "\n";
    EXPECT_GE(val, new_val);
}

TEST(SocketOptionBuffer, SndBuf) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, &len);
    std::cout << "SO_SNDBUF default: " << val << "\n";
    EXPECT_GT(val, 0);

    int new_val = 65536;
    std::cout << "SO_SNDBUF set    : " << new_val << "\n";
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &new_val, sizeof(new_val));

    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, &len);
    std::cout << "SO_SNDBUF after  : " << val << "\n";
    EXPECT_GE(val, new_val);

    close(fd);
}

TEST(SocketOptionBuffer, ZeroSndBuf) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, &len);
    std::cout << "SO_SNDBUF default: " << val << "\n";
    EXPECT_GT(val, 0);

    int new_val = 0;
    std::cout << "SO_SNDBUF set    : " << new_val << "\n";
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &new_val, sizeof(new_val));

    getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, &len);
    std::cout << "SO_SNDBUF after  : " << val << "\n";
    EXPECT_GE(val, new_val);

    close(fd);
}

// -------- Timeout --------

TEST(SocketOptionTimeout, RcvTimeo) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    timeval tv{};
    socklen_t len = sizeof(tv);
    getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len);
    std::cout << "SO_RCVTIMEO default: " << tv.tv_sec << "s " << tv.tv_usec
              << "us\n";
    EXPECT_EQ(tv.tv_sec, 0);
    EXPECT_EQ(tv.tv_usec, 0);

    timeval new_tv{};
    new_tv.tv_sec = 2;
    new_tv.tv_usec = 0;
    std::cout << "SO_RCVTIMEO set    : " << new_tv.tv_sec << "s "
              << new_tv.tv_usec << "us\n";
    EXPECT_EQ(
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &new_tv, sizeof(new_tv)), 0
    );

    getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len);
    std::cout << "SO_RCVTIMEO after  : " << tv.tv_sec << "s " << tv.tv_usec
              << "us\n";
    EXPECT_EQ(tv.tv_sec, 2);

    close(fd);
}

TEST(SocketOptionTimeout, SndTimeo) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    timeval tv{};
    socklen_t len = sizeof(tv);
    getsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, &len);
    std::cout << "SO_SNDTIMEO default: " << tv.tv_sec << "s " << tv.tv_usec
              << "us\n";
    EXPECT_EQ(tv.tv_sec, 0);
    EXPECT_EQ(tv.tv_usec, 0);

    timeval new_tv{};
    new_tv.tv_sec = 3;
    new_tv.tv_usec = 0;
    std::cout << "SO_SNDTIMEO set    : " << new_tv.tv_sec << "s "
              << new_tv.tv_usec << "us\n";
    EXPECT_EQ(
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &new_tv, sizeof(new_tv)), 0
    );

    getsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, &len);
    std::cout << "SO_SNDTIMEO after  : " << tv.tv_sec << "s " << tv.tv_usec
              << "us\n";
    EXPECT_EQ(tv.tv_sec, 3);

    close(fd);
}

// -------- Connection --------

TEST(SocketOptionConnection, KeepAlive) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, &len);
    std::cout << "SO_KEEPALIVE default: " << val << "\n";
    EXPECT_EQ(val, 0);

    int on = 1;
    std::cout << "SO_KEEPALIVE set    : " << on << "\n";
    EXPECT_EQ(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)), 0);

    getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, &len);
    std::cout << "SO_KEEPALIVE after  : " << val << "\n";
    EXPECT_EQ(val, 1);

    close(fd);
}

TEST(SocketOptionConnection, Linger) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    linger lg{};
    socklen_t len = sizeof(lg);
    getsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, &len);
    std::cout << "SO_LINGER default: onoff=" << lg.l_onoff
              << " linger=" << lg.l_linger << "\n";
    EXPECT_EQ(lg.l_onoff, 0);

    linger new_lg{};
    new_lg.l_onoff = 1;
    new_lg.l_linger = 5;
    std::cout << "SO_LINGER set    : onoff=" << new_lg.l_onoff
              << " linger=" << new_lg.l_linger << "\n";
    EXPECT_EQ(
        setsockopt(fd, SOL_SOCKET, SO_LINGER, &new_lg, sizeof(new_lg)), 0
    );

    getsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, &len);
    std::cout << "SO_LINGER after  : onoff=" << lg.l_onoff
              << " linger=" << lg.l_linger << "\n";
    EXPECT_EQ(lg.l_onoff, 1);
    EXPECT_EQ(lg.l_linger, 5);

    close(fd);
}

TEST(SocketOptionConnection, Broadcast) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_BROADCAST, &val, &len);
    std::cout << "SO_BROADCAST default: " << val << "\n";
    EXPECT_EQ(val, 0);

    int on = 1;
    std::cout << "SO_BROADCAST set    : " << on << "\n";
    EXPECT_EQ(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)), 0);

    getsockopt(fd, SOL_SOCKET, SO_BROADCAST, &val, &len);
    std::cout << "SO_BROADCAST after  : " << val << "\n";
    EXPECT_EQ(val, 1);

    close(fd);
}

// -------- Read-only --------

TEST(SocketOptionReadOnly, AcceptConn) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len);
    std::cout << "SO_ACCEPTCONN before listen: " << val << "\n";
    EXPECT_EQ(val, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    EXPECT_NE(bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)), -1);
    EXPECT_NE(listen(fd, 5), -1);

    getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len);
    std::cout << "SO_ACCEPTCONN after listen:  " << val << "\n";
    EXPECT_EQ(val, 1);

    close(fd);
}

TEST(SocketOptionReadOnly, Type) {
    int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(tcp_fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(tcp_fd, SOL_SOCKET, SO_TYPE, &val, &len);
    std::cout << "SO_TYPE (tcp): " << val << " (SOCK_STREAM=" << SOCK_STREAM
              << ")\n";
    EXPECT_EQ(val, SOCK_STREAM);
    close(tcp_fd);

    int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_NE(udp_fd, -1);

    getsockopt(udp_fd, SOL_SOCKET, SO_TYPE, &val, &len);
    std::cout << "SO_TYPE (udp): " << val << " (SOCK_DGRAM=" << SOCK_DGRAM
              << ")\n";
    EXPECT_EQ(val, SOCK_DGRAM);
    close(udp_fd);
}

TEST(SocketOptionReadOnly, Domain) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &val, &len);
    std::cout << "SO_DOMAIN: " << val << " (AF_INET=" << AF_INET << ")\n";
    EXPECT_EQ(val, AF_INET);

    close(fd);
}

TEST(SocketOptionReadOnly, Protocol) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &val, &len);
    std::cout << "SO_PROTOCOL: " << val << " (IPPROTO_TCP=" << IPPROTO_TCP
              << ")\n";
    EXPECT_EQ(val, IPPROTO_TCP);

    close(fd);
}

TEST(SocketOptionReadOnly, Error) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    int val = 0;
    socklen_t len = sizeof(val);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &len);
    std::cout << "SO_ERROR: " << val << "\n";
    EXPECT_EQ(val, 0);

    close(fd);
}
