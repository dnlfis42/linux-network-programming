#include "config.hpp"
#include "error.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <random>
#include <string>
#include <thread>

std::atomic<bool> g_running{true};

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

    // ---- 서버로부터 id 수신 ----
    char id_buf[16]{};
    ssize_t id_recv = recv(fd, id_buf, sizeof(id_buf), 0);
    if (id_recv <= 0) {
        log_err(errno, "failed to receive id");
        close(fd);
        return 0;
    }
    id_buf[id_recv] = '\0';
    std::string my_id(id_buf);
    log_info("my id: ", my_id);

    // ---- 수신 스레드 ----
    std::thread recv_thread([fd] {
        char buf[MAX_BUFLEN]{};
        while (g_running) {
            ssize_t r_recv = recv(fd, buf, MAX_BUFLEN, 0);
            if (r_recv == -1) {
                if (errno == EINTR)
                    continue;
                log_err(errno, "recv() failed");
                g_running = false;
                break;
            }
            if (r_recv == 0) {
                log_info("server disconnected");
                g_running = false;
                break;
            }
            buf[r_recv] = '\0';
            log_info("recv: ", buf);
        }
    });

    // ---- 송신 루프 (하트비트) ----
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(1, 5);
    int count = 0;

    while (g_running) {
        int delay = dist(rng);
        for (int i = 0; i < delay * 10 && g_running; ++i) {
            usleep(100000); // 100ms 단위로 체크
        }
        if (!g_running)
            break;

        std::string msg =
            "heartbeat #" + std::to_string(++count) + " from id=" + my_id;
        ssize_t r_send = send(fd, msg.c_str(), msg.size(), 0);
        if (r_send == -1) {
            log_err(errno, "send() failed");
            g_running = false;
            break;
        }
        log_info("send: ", msg);
    }

    // ---- 정리 ----
    close(fd);
    recv_thread.join();

    return 0;
}
