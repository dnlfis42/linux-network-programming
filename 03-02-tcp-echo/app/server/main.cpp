#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;

void handle_signal(int) { running = 0; }

int main() {
    // SIGPIPE 무시 — 클라이언트가 끊어도 서버가 죽지 않게
    signal(SIGPIPE, SIG_IGN);
    // SIGINT/SIGTERM — graceful shutdown (SA_RESTART 없이)
    struct sigaction sa{};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // SA_RESTART 없음 → recv()가 EINTR 반환
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    // 소켓 생성
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        log_err("socket() failed");
        return 0;
    }
    // 주소 설정
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 소켓-주소 바인딩
    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        log_err("bind() failed");
        close(fd);
        return 0;
    }
    // 리슨
    if (listen(fd, SOMAXCONN) == -1) {
        log_err("listen() failed");
        close(fd);
        return 0;
    }
    // 서버 IP 문자열 변환
    char server_ip[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &addr.sin_addr, server_ip, INET_ADDRSTRLEN) ==
        nullptr) {
        log_err("inet_ntop() failed");
        close(fd);
        return 0;
    }
    // 서버 동작 로그
    log_info("server listening on ", server_ip, ":", PORT);
    // 클라이언트 주소 공간
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    // 클라이언트 접속
    int client_fd =
        accept(fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    if (client_fd == -1) {
        log_err("accept() failed");
        close(fd);
        return 0;
    }
    // 클라이언트 IP 문자열 변환
    char client_ip[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) ==
        nullptr) {
        log_err("inet_ntop(client) failed");
        close(client_fd);
        close(fd);
        return 0;
    }
    // 클라이언트 정보 로그
    log_info(
        "client connected\nfd  : ", client_fd, "\naddr: ", client_ip,
        "\nport: ", ntohs(client_addr.sin_port)
    );
    //
    while (running) {
        char buf[1024]{};
        // 메시지 송신
        ssize_t r_recv = recv(client_fd, buf, sizeof(buf), 0);
        if (r_recv == -1) {
            if (errno == EINTR) {
                continue; // 시그널에 의한 중단, 재시도
            }
            log_err("recv() failed");
            break;
        } else if (r_recv == 0) {
            log_info("client disconnected");
            break;
        } else {
            log_info("recv msg\nlen: ", r_recv, "\nmsg: ", buf);
            // 재전송
            ssize_t r_send = send(client_fd, buf, r_recv, MSG_NOSIGNAL);
            if (r_send == -1) {
                log_err("send() failed");
                break;
            } else {
                log_info(
                    "send msg\nrecv size: ", r_recv, "\nsend size: ", r_send
                );
            }
        }
    }
    log_info("server shutting down");
    // linger 설정
    linger lg{1, 0};
    if (setsockopt(client_fd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg)) == -1) {
        log_err("setsockopt(client,linger) failed");
    }
    // fd 닫기
    close(client_fd);
    close(fd);
    // 프로그램 종료
    return 0;
}