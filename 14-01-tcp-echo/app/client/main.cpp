#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

int main() {
    // ---- 소켓 생성 ----
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        log_err(errno, "socket() failed");
        return 0;
    }

    // ---- 서버 주소 설정 ----
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // ---- 서버 연결 ----
    if (connect(
            fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)
        ) == -1) {
        log_err(errno, "connect() failed");
        close(fd);
        return 0;
    }
    log_info("connected to 127.0.0.1:", PORT);

    // ---- 에코 루프 ----
    log_info("type 'q' to quit");
    std::string msg;
    while (std::getline(std::cin, msg)) {
        if (msg == "q")
            break;
        // 송신
        ssize_t r_send = send(fd, msg.c_str(), msg.size(), 0);
        if (r_send == -1) {
            log_err(errno, "send() failed");
            break;
        }
        log_info("send: len=", r_send);

        // 수신
        char buf[MAX_BUFLEN]{};
        ssize_t r_recv = recv(fd, buf, MAX_BUFLEN, 0);
        if (r_recv == -1) {
            log_err(errno, "recv() failed");
            break;
        }
        if (r_recv == 0) {
            log_info("server disconnected");
            break;
        }
        log_info("recv: len=", r_recv, " msg=", buf);
    }

    // ---- 정리 ----
    close(fd);

    return 0;
}
