#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // 소켓 생성
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        log_err("socket() failed");
        return 0;
    }
    // 서버 주소 설정
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // 메시지 송신
    const char* msg = "Hello UDP~";
    ssize_t n = sendto(
        fd, msg, strlen(msg), 0, reinterpret_cast<sockaddr*>(&server_addr),
        sizeof(server_addr)
    );
    if (n == -1) {
        log_err("sendto() failed");
    } else {
        log_info("send msg: ", msg, "\nsend size: ", n);
    }
    // fd 닫기
    close(fd);

    return 0;
}
