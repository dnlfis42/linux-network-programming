#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

// -------- epoll 생성 --------

TEST(Epoll, Create) {
    int epfd = epoll_create1(0);
    ASSERT_NE(epfd, -1);
    std::cout << "epoll fd: " << epfd << "\n";

    close(epfd);
}

TEST(Epoll, CreateWithCloexec) {
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    ASSERT_NE(epfd, -1);

    int flags = fcntl(epfd, F_GETFD);
    std::cout << "EPOLL_CLOEXEC: FD_CLOEXEC = "
              << (flags & FD_CLOEXEC ? "on" : "off") << "\n";
    EXPECT_NE(flags & FD_CLOEXEC, 0);

    close(epfd);
}

// -------- epoll_ctl --------

TEST(Epoll, AddAndRemoveFd) {
    int epfd = epoll_create1(0);
    ASSERT_NE(epfd, -1);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 등록
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    EXPECT_EQ(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev), 0);
    std::cout << "EPOLL_CTL_ADD: fd " << fd << " registered\n";

    // 수정
    ev.events = EPOLLIN | EPOLLOUT;
    EXPECT_EQ(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev), 0);
    std::cout << "EPOLL_CTL_MOD: fd " << fd
              << " modified to EPOLLIN|EPOLLOUT\n";

    // 삭제
    EXPECT_EQ(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr), 0);
    std::cout << "EPOLL_CTL_DEL: fd " << fd << " removed\n";

    close(fd);
    close(epfd);
}

TEST(Epoll, AddDuplicateFails) {
    int epfd = epoll_create1(0);
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    // 같은 fd 중복 등록 → EEXIST
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(errno, EEXIST);
    std::cout << "duplicate ADD: " << strerror(errno) << " (" << errno << ")\n";

    close(fd);
    close(epfd);
}

// -------- epoll_wait --------

TEST(Epoll, WaitTimeout) {
    int epfd = epoll_create1(0);
    ASSERT_NE(epfd, -1);

    epoll_event events[64];
    // 등록된 fd 없이 100ms 타임아웃
    int n = epoll_wait(epfd, events, 64, 100);
    EXPECT_EQ(n, 0);
    std::cout << "epoll_wait timeout: returned " << n << " events\n";

    close(epfd);
}

TEST(Epoll, DetectsReadableEvent) {
    // 서버 준비
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    // epoll에 서버 소켓 등록
    int epfd = epoll_create1(0);
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

    // 클라이언트 연결
    std::thread client([&addr] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(fd, (sockaddr*)&addr, sizeof(addr));
        send(fd, "hello", 5, 0);
        usleep(100000);
        close(fd);
    });

    // accept 이벤트 감지
    epoll_event events[64];
    int n = epoll_wait(epfd, events, 64, 1000);
    EXPECT_GT(n, 0);
    EXPECT_EQ(events[0].data.fd, server_fd);
    std::cout << "EPOLLIN on server_fd: new connection ready\n";

    // accept
    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);

    // client_fd를 epoll에 등록
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

    // 데이터 수신 이벤트 감지
    n = epoll_wait(epfd, events, 64, 1000);
    EXPECT_GT(n, 0);
    EXPECT_EQ(events[0].data.fd, client_fd);

    char buf[64]{};
    ssize_t r = recv(client_fd, buf, sizeof(buf), 0);
    EXPECT_GT(r, 0);
    std::cout << "EPOLLIN on client_fd: recv \"" << buf << "\"\n";

    close(client_fd);
    close(server_fd);
    close(epfd);
    client.join();
}

// -------- 이벤트 플래그 --------

TEST(Epoll, EventFlags) {
    std::cout << "EPOLLIN:    " << EPOLLIN << "\n";
    std::cout << "EPOLLOUT:   " << EPOLLOUT << "\n";
    std::cout << "EPOLLERR:   " << EPOLLERR << "\n";
    std::cout << "EPOLLHUP:   " << EPOLLHUP << "\n";
    std::cout << "EPOLLRDHUP: " << EPOLLRDHUP << "\n";
    std::cout << "EPOLLET:    " << EPOLLET << "\n";
    std::cout << "EPOLLONESHOT: " << EPOLLONESHOT << "\n";
}

// -------- EPOLLRDHUP — 상대방 close 감지 --------

TEST(Epoll, DetectsRdhup) {
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    // 클라이언트: 연결 후 바로 닫기
    std::thread client([&addr] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(fd, (sockaddr*)&addr, sizeof(addr));
        usleep(50000);
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

    int epfd = epoll_create1(0);
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLRDHUP;
    ev.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

    epoll_event events[64];
    int n = epoll_wait(epfd, events, 64, 1000);
    EXPECT_GT(n, 0);

    if (events[0].events & EPOLLRDHUP) {
        std::cout << "EPOLLRDHUP: peer closed connection\n";
    }
    if (events[0].events & EPOLLIN) {
        std::cout << "EPOLLIN: also triggered (recv would return 0)\n";
    }

    close(client_fd);
    close(server_fd);
    close(epfd);
    client.join();
}

// -------- Edge-Triggered vs Level-Triggered --------

TEST(Epoll, LevelTriggered) {
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    std::thread client([&addr] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(fd, (sockaddr*)&addr, sizeof(addr));
        send(fd, "hello world!", 12, 0);
        usleep(200000);
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

    int epfd = epoll_create1(0);
    epoll_event ev{};
    ev.events = EPOLLIN; // LT 모드 (기본)
    ev.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

    usleep(50000); // 데이터 도착 대기

    // 1바이트만 읽기
    epoll_event events[64];
    int n = epoll_wait(epfd, events, 64, 1000);
    EXPECT_GT(n, 0);

    char buf[1]{};
    recv(client_fd, buf, 1, 0);
    std::cout << "LT: read 1 byte: '" << buf[0] << "'\n";

    // LT 모드: 데이터 남아있으니 다시 알림
    n = epoll_wait(epfd, events, 64, 100);
    EXPECT_GT(n, 0);
    std::cout << "LT: epoll_wait again → " << n
              << " event(s) (data still available)\n";

    close(client_fd);
    close(server_fd);
    close(epfd);
    client.join();
}

TEST(Epoll, EdgeTriggered) {
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_NE(server_fd, -1);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    socklen_t len = sizeof(addr);
    getsockname(server_fd, (sockaddr*)&addr, &len);
    listen(server_fd, 5);

    std::thread client([&addr] {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        connect(fd, (sockaddr*)&addr, sizeof(addr));
        send(fd, "hello world!", 12, 0);
        usleep(200000);
        close(fd);
    });

    int client_fd = accept(server_fd, nullptr, nullptr);
    ASSERT_NE(client_fd, -1);
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL, 0) | O_NONBLOCK);

    int epfd = epoll_create1(0);
    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET; // ET 모드
    ev.data.fd = client_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

    usleep(50000); // 데이터 도착 대기

    // 1바이트만 읽기
    epoll_event events[64];
    int n = epoll_wait(epfd, events, 64, 1000);
    EXPECT_GT(n, 0);

    char buf[1]{};
    recv(client_fd, buf, 1, 0);
    std::cout << "ET: read 1 byte: '" << buf[0] << "'\n";

    // ET 모드: 상태 변화 없으니 알림 없음 → 타임아웃
    n = epoll_wait(epfd, events, 64, 100);
    EXPECT_EQ(n, 0);
    std::cout << "ET: epoll_wait again → " << n << " event(s) (no new edge)\n";

    close(client_fd);
    close(server_fd);
    close(epfd);
    client.join();
}
