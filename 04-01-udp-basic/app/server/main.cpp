#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
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
    // 주소 설정
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // 소켓-주소 바인딩
    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        log_err("bind() failed");
        close(fd);
        return 0;
    }

    log_info("udp server listening on port ", PORT);

    // 클라이언트 주소 공간
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);
    // 버퍼 준비
    char buf[64]{};
    // 메시지 수신
    ssize_t n = recvfrom(
        fd, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&client_addr),
        &addr_len
    );
    if (n == -1) {
        log_err("recvfrom() failed");
    } else {
        // 클라이언트 ip 문자열 변환
        char client_ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        log_info(
            "from ", client_ip, ":", ntohs(client_addr.sin_port),
            "\nmsg : ", buf
        );
    }
    // fd 닫기
    close(fd);

    return 0;
}
