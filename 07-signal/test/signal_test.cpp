#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

// -------- signal() --------

TEST(Signal, IgnoreSigpipe) {
    // SIGPIPE를 무시하도록 설정
    auto prev = signal(SIGPIPE, SIG_IGN);
    EXPECT_NE(prev, SIG_ERR);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 미연결 소켓에 send — SIGPIPE 발생하지 않고 errno만 설정됨
    ssize_t n = send(fd, "hello", 5, 0);
    EXPECT_EQ(n, -1);
    std::cout << "send on unconnected (SIGPIPE ignored): " << strerror(errno)
              << " (" << errno << ")\n";

    close(fd);
    signal(SIGPIPE, SIG_DFL); // 복원
}

TEST(Signal, MsgNosignalPreventsSigpipe) {
    // MSG_NOSIGNAL 플래그로 SIGPIPE 방지 (signal 설정 불필요)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    ssize_t n = send(fd, "hello", 5, MSG_NOSIGNAL);
    EXPECT_EQ(n, -1);
    std::cout << "send with MSG_NOSIGNAL: " << strerror(errno) << " (" << errno
              << ")\n";

    close(fd);
}

TEST(Signal, CustomHandler) {
    static std::atomic<int> caught{0};

    // 커스텀 핸들러 등록
    auto prev = signal(SIGUSR1, [](int sig) { caught.store(sig); });
    EXPECT_NE(prev, SIG_ERR);

    // 자기 자신에게 시그널 전송
    raise(SIGUSR1);

    EXPECT_EQ(caught.load(), SIGUSR1);
    std::cout << "SIGUSR1 (" << SIGUSR1 << ") caught by custom handler\n";

    signal(SIGUSR1, SIG_DFL); // 복원
}

TEST(Signal, SignalDefaultValues) {
    // 주요 시그널 번호 확인
    std::cout << "SIGINT:   " << SIGINT << "\n";
    std::cout << "SIGTERM:  " << SIGTERM << "\n";
    std::cout << "SIGPIPE:  " << SIGPIPE << "\n";
    std::cout << "SIGKILL:  " << SIGKILL << "\n";
    std::cout << "SIGCHLD:  " << SIGCHLD << "\n";
    std::cout << "SIGUSR1:  " << SIGUSR1 << "\n";
    std::cout << "SIGUSR2:  " << SIGUSR2 << "\n";

    // SIGKILL은 무시/핸들링 불가
    auto ret = signal(SIGKILL, SIG_IGN);
    EXPECT_EQ(ret, SIG_ERR);
    std::cout << "signal(SIGKILL, SIG_IGN): SIG_ERR (무시 불가)\n";
}

// -------- sigaction() --------

TEST(Signal, SigactionBasic) {
    static std::atomic<int> caught{0};

    struct sigaction sa{};
    sa.sa_handler = [](int sig) { caught.store(sig); };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int ret = sigaction(SIGUSR2, &sa, nullptr);
    EXPECT_EQ(ret, 0);

    raise(SIGUSR2);

    EXPECT_EQ(caught.load(), SIGUSR2);
    std::cout << "SIGUSR2 (" << SIGUSR2 << ") caught by sigaction handler\n";

    // 복원
    sa.sa_handler = SIG_DFL;
    sigaction(SIGUSR2, &sa, nullptr);
}

TEST(Signal, SigactionWithSaRestart) {
    // SA_RESTART: 시그널에 의해 중단된 시스템 콜을 자동 재시작
    struct sigaction sa{};
    sa.sa_handler = [](int) {}; // 빈 핸들러
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    int ret = sigaction(SIGUSR1, &sa, nullptr);
    EXPECT_EQ(ret, 0);
    std::cout
        << "SA_RESTART set for SIGUSR1 — interrupted syscalls auto-restart\n";

    // 복원
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, nullptr);
}

TEST(Signal, SigactionGetPrevious) {
    // 이전 핸들러 확인
    struct sigaction sa{};
    sa.sa_handler = [](int) {};
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    struct sigaction old_sa{};
    sigaction(SIGUSR1, &sa, &old_sa);

    std::cout << "previous SIGUSR1 handler: "
              << (old_sa.sa_handler == SIG_DFL ? "SIG_DFL" : "custom") << "\n";

    // 복원
    sigaction(SIGUSR1, &old_sa, nullptr);
}

TEST(Signal, SigactionMaskBlocksSignal) {
    static std::atomic<int> usr1_count{0};
    static std::atomic<int> usr2_count{0};

    // SIGUSR1 핸들러: 실행 중 SIGUSR2를 블록
    struct sigaction sa1{};
    sa1.sa_handler = [](int) {
        usr1_count++;
        // 핸들러 실행 중 SIGUSR2가 블록됨
        raise(SIGUSR2); // 핸들러 끝나고 나서 전달됨
    };
    sigemptyset(&sa1.sa_mask);
    sigaddset(&sa1.sa_mask, SIGUSR2); // SIGUSR2 블록
    sa1.sa_flags = 0;

    struct sigaction sa2{};
    sa2.sa_handler = [](int) { usr2_count++; };
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    sigaction(SIGUSR1, &sa1, nullptr);
    sigaction(SIGUSR2, &sa2, nullptr);

    raise(SIGUSR1);

    std::cout << "SIGUSR1 caught: " << usr1_count.load() << "\n";
    std::cout << "SIGUSR2 caught: " << usr2_count.load() << "\n";
    EXPECT_EQ(usr1_count.load(), 1);
    EXPECT_EQ(usr2_count.load(), 1); // 블록 해제 후 전달됨

    // 복원
    sa1.sa_handler = SIG_DFL;
    sa2.sa_handler = SIG_DFL;
    sigaction(SIGUSR1, &sa1, nullptr);
    sigaction(SIGUSR2, &sa2, nullptr);
}

// -------- 소켓에서의 시그널 --------

TEST(Signal, InterruptedRecvBySignal) {
    // SA_RESTART 없이 시그널이 recv()를 중단시키는 예시
    struct sigaction sa{};
    sa.sa_handler = [](int) {};
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // SA_RESTART 없음

    sigaction(SIGUSR1, &sa, nullptr);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(fd, -1);

    // 타임아웃 설정 (안전장치)
    timeval tv{};
    tv.tv_sec = 1;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // 별도 스레드에서 시그널 전송
    auto main_tid = pthread_self();
    std::thread t([main_tid] {
        usleep(100000); // 100ms 대기
        pthread_kill(main_tid, SIGUSR1);
    });

    // bind + listen
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    bind(fd, (sockaddr*)&addr, sizeof(addr));
    listen(fd, 1);

    // accept가 시그널에 의해 중단됨
    int client_fd = accept(fd, nullptr, nullptr);
    int saved_errno = errno;

    if (client_fd == -1 && saved_errno == EINTR) {
        std::cout << "accept() interrupted by signal: " << strerror(saved_errno)
                  << " (" << saved_errno << ")\n";
    } else {
        std::cout << "accept() returned: " << client_fd
                  << " errno: " << strerror(saved_errno) << "\n";
    }

    t.join();
    if (client_fd != -1)
        close(client_fd);
    close(fd);

    // 복원
    sa.sa_handler = SIG_DFL;
    sigaction(SIGUSR1, &sa, nullptr);
}
