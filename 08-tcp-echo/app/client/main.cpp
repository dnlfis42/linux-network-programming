#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // 소켓 생성
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        log_err("socket() failed");
        return 0;
    }
    // 서버 주소 설정
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // 서버-클라 연결
    if (connect(
            fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)
        ) == -1) {
        log_err("connect() failed");
        close(fd);
        return 0;
    }
    // 서버 IP 문자열 변환
    char server_ip[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN) ==
        nullptr) {
        log_err("inet_ntop() failed");
        close(fd);
        return 0;
    }
    // 서버 정보 로그
    log_info(
        "server connected\naddr: ", server_ip,
        "\nport: ", ntohs(server_addr.sin_port)
    );
    //
    bool is_running = true;
    while (is_running) {
        // 메시지 작성
        char msg[1024]{};
        std::cin.getline(msg, sizeof(msg));
        size_t msg_len = strlen(msg);
        // 메시지 송신
        ssize_t r_send = send(fd, msg, msg_len, 0);
        if (r_send == -1) {
            log_err("send() failed");
            is_running = false;
        } else {
            log_info("send msg\nmsg len  : ", msg_len, "\nsend size: ", r_send);
            // 메시지 수신
            char buf[1024]{};
            ssize_t r_recv = recv(fd, buf, sizeof(buf), 0);
            if (r_recv == -1) {
                log_err("recv() failed");
                is_running = false;
            } else if (r_recv == 0) {
                log_info("server disconnected");
                is_running = false;
            } else {
                log_info("recv msg\nlen: ", r_recv, "\nmsg: ", buf);
            }
        }
    }
    // fd 닫기
    close(fd);
    // 프로그램 종료
    return 0;
}