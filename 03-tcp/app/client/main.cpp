#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
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
    // 서버 ip 문자열 변환
    char server_ip[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, INET_ADDRSTRLEN) ==
        nullptr) {
        log_err("inet_ntop() failed");
        close(fd);
        return 0;
    }
    // 서버 정보 출력
    log_info(
        "[server info]\naddr: ", server_ip,
        "\nport: ", ntohs(server_addr.sin_port)
    );
    // 버퍼 준비
    char buf[64]{"Hello there~"};
    size_t buf_len = strlen(buf);
    // 메시지 송신
    ssize_t r_send = send(fd, buf, buf_len, 0);
    if (r_send == -1) {
        log_err("send() failed");
    } else {
        log_info(
            "send msg: ", buf, "\nbuf len: ", buf_len, "\nsend size: ", r_send
        );
    }
    // fd 닫기
    close(fd);
    // 프로그램 종료
    return 0;
}
