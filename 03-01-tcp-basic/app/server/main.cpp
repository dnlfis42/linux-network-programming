#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
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
    // 클라이언트 주소 공간
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    // 클라이언트 accept
    int client_fd =
        accept(fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
    if (client_fd == -1) {
        log_err("accept() failed");
        close(fd);
        return 0;
    }
    // 클라이언트 ip 문자열 변환
    char client_ip[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) ==
        nullptr) {
        log_err("inet_ntop() failed");
        close(client_fd);
        close(fd);
        return 0;
    }
    // 클라이언트 정보 출력
    log_info(
        "client connected\n", "fd  : ", client_fd, "\naddr: ", client_ip,
        "\nport: ", ntohs(client_addr.sin_port)
    );
    // 버퍼 준비
    char buf[64]{};
    // 메시지 수신
    ssize_t r_recv = recv(client_fd, buf, sizeof(buf), 0);
    if (r_recv == -1) {
        log_err("recv() failed");
    } else if (r_recv == 0) {
        log_info("client ", client_fd, " disconnected");
    } else {
        // 메시지 출력
        log_info("client msg: ", buf);
    }
    // fd 닫기
    close(client_fd);
    close(fd);
    // 프로그램 종료
    return 0;
}