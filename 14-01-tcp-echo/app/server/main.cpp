#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>

constexpr int MAX_EVENTS = 64;

volatile sig_atomic_t g_running = 1;

void handle_signal(int) { g_running = 0; }

int main() {
    // ---- 시그널 설정 ----
    signal(SIGPIPE, SIG_IGN);

    // ---- 소켓 생성 ----
    int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd == -1) {
        log_err(errno, "socket() failed");
        return 0;
    }

    // ---- 주소 설정 ----
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // ---- 바인딩 ----
    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        log_err(errno, "bind() failed");
        close(fd);
        return 0;
    }

    // ---- 리스닝 ----
    if (listen(fd, SOMAXCONN) == -1) {
        log_err(errno, "listen() failed");
        close(fd);
        return 0;
    }

    // ---- epoll 생성 ----
    int epfd = epoll_create1(0);
    if (epfd == -1) {
        log_err(errno, "epoll_create1() failed");
        close(fd);
        return 0;
    }

    // ---- listen 소켓을 epoll에 등록 ----
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        log_err(errno, "epoll_ctl() failed");
        close(epfd);
        close(fd);
        return 0;
    }

    // ---- 이벤트 루프 ----
    std::array<epoll_event, MAX_EVENTS> events;
    int client_fd;

    while (g_running) {
        int nfds = epoll_wait(epfd, events.data(), MAX_EVENTS, -1);
        // epoll_wait 실패
        if (nfds == -1) {
            if (errno == EINTR)
                continue; // 시그널 중단 → 재시도
            log_err(errno, "epoll_wait() failed");
            break;
        }

        for (int n = 0; n < nfds; ++n) {
            // ---- 새 연결 (listen 소켓) ----
            if (events[n].data.fd == fd) {
                sockaddr_in client_addr{};
                socklen_t addr_len = sizeof(client_addr);
                client_fd = accept4(
                    fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len,
                    SOCK_NONBLOCK
                );
                // accept4 실패
                if (client_fd == -1) {
                    int e = errno;
                    if (e == EAGAIN || e == ECONNABORTED) {
                        // 대기 연결 없음 / 연결 즉시 끊김 → 무시
                    } else if (e == EMFILE || e == ENFILE) {
                        log_err(e, "accept4() fd limit"); // fd 한도 초과
                    } else {
                        log_err(e, "accept4() failed"); // 심각한 에러
                    }
                    continue;
                }
                // 클라이언트를 epoll에 등록
                epoll_event client_ev{};
                client_ev.events = EPOLLIN | EPOLLRDHUP;
                client_ev.data.fd = client_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev) ==
                    -1) {
                    log_err(errno, "epoll_ctl(ADD) failed");
                    close(client_fd);
                    continue;
                }
                // 클라이언트 정보 출력
                char client_ip[INET_ADDRSTRLEN]{};
                if (inet_ntop(
                        AF_INET, &client_addr.sin_addr, client_ip,
                        INET_ADDRSTRLEN
                    ) == nullptr) {
                    log_info("client connected: fd=", client_fd);
                } else {
                    log_info(
                        "client connected: fd=", client_fd, " ip=", client_ip,
                        " port=", ntohs(client_addr.sin_port)
                    );
                }
            }
            // ---- 클라이언트 종료 ----
            else if (events[n].events & EPOLLRDHUP) {
                client_fd = events[n].data.fd;
                log_info("client disconnected: fd=", client_fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, nullptr);
                close(client_fd);
            }
            // ---- 데이터 도착 ----
            else if (events[n].events & EPOLLIN) {
                client_fd = events[n].data.fd;
                char buf[MAX_BUFLEN]{};

                // 수신
                ssize_t r_recv = recv(client_fd, buf, MAX_BUFLEN, 0);
                if (r_recv == -1) {
                    int e = errno;
                    if (e == EAGAIN)
                        continue; // 데이터 없음 → 무시
                    log_err(e, "recv() failed");
                    continue;
                }
                if (r_recv == 0) {
                    // 연결 종료 (EPOLLRDHUP 안 온 경우)
                    log_info("client disconnected: fd=", client_fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, nullptr);
                    close(client_fd);
                    continue;
                }

                // 에코 송신
                log_info("recv: fd=", client_fd, " len=", r_recv, " msg=", buf);
                ssize_t r_send = send(client_fd, buf, r_recv, 0);
                if (r_send == -1) {
                    int e = errno;
                    if (e == EAGAIN) {
                        log_warn("send() EAGAIN: fd=", client_fd);
                    } else {
                        log_err(e, "send() failed");
                        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        close(client_fd);
                    }
                } else {
                    log_info("send: fd=", client_fd, " len=", r_send);
                }
            }
        }
    }

    // ---- 정리 ----
    close(epfd);
    close(fd);

    return 0;
}
