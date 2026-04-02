# Linux Network Programming

Linux 소켓(Socket) 프로그래밍을 단계별로 학습하는 프로젝트.

## 환경

|            | 버전                       |
| ---------- | -------------------------- |
| Ubuntu     | 24.04.4 LTS (Noble Numbat) |
| gcc        | 13.3.0                     |
| cmake      | 3.28.3                     |
| gtest      | 1.14.0                     |
| code (vsc) | 1.113.0                    |

| vsc 확장    | 용도                     |
| ----------- | ------------------------ |
| C/C++       | 디버깅                   |
| CMake Tools | CMake 빌드/설정 IDE 연동 |
| clangd      | 코드 자동 완성, 린팅     |

## 빌드 및 테스트

```bash
cmake -B build
cmake --build build

# 전체 테스트
ctest --test-dir build

# 프로젝트별 테스트
ctest --test-dir build/00-socket/test --output-on-failure

# 상세 출력
ctest --test-dir build/00-socket/test -V
```

## 구조

| 디렉터리           | 주제                     | 주요 등장 함수                                                              | 완료 |
| ------------------ | ------------------------ | --------------------------------------------------------------------------- | :--: |
| 00-socket          | 소켓 생성과 종료         | `socket()`, `close()`                                                       |  O   |
| 01-address         | 주소 체계와 변환         | `htons()`, `htonl()`, `ntohs()`, `ntohl()`,<br>`inet_pton()`, `inet_ntop()` |  O   |
| 02-dns             | 이름 해석                | `getaddrinfo()`, `getnameinfo()`, `freeaddrinfo()`                          |  O   |
| 03-tcp             | TCP 통신                 | `bind()`, `listen()`, `accept()`, `connect()`,<br>`send()`, `recv()`        |  O   |
| 04-udp             | UDP 통신                 | `sendto()`, `recvfrom()`                                                    |  O   |
| 05-option          | 소켓 옵션                | `setsockopt()`, `getsockopt()`                                              |  O   |
| 06-error           | 소켓 에러 처리           | `strerror()`, `perror()`, `gai_strerror()`                                  |  O   |
| 07-signal          | 시그널 처리              | `signal()`, `sigaction()`                                                   |  O   |
| 08-tcp-echo        | TCP 에코 서버            |                                                                             |  O   |
| 09-udp-echo        | UDP 에코 서버            |                                                                             |      |
| 10-nonblocking     | 논블로킹 소켓            | `fcntl()`, `ioctl()`                                                        |  O   |
| 11-io-multiplexing | I/O 멀티플렉싱           |                                                                             |  O   |
| 12-select          | select                   | `select()`, `FD_SET()`                                                      |      |
| 13-poll            | poll                     | `poll()`                                                                    |      |
| 14-00-epoll        | epoll                    | `epoll_create1()`, `epoll_ctl()`, `epoll_wait()`                            |  O   |
| 14-01-tcp-echo     | epoll 기반 TCP 에코 서버 |                                                                             |      |
| 14-02-tcp-chat     | epoll 기반 TCP 채팅 서버 |                                                                             |      |

각 디렉토리는 `test/` 폴더에서 GTest로 함수와 구조체를 직접 사용해보는 방식으로 구성.
