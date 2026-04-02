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

| 디렉터리           | 주제             | 주요 등장 함수                                                              | 완료 |
| ------------------ | ---------------- | --------------------------------------------------------------------------- | :--: |
| 00-socket          | 소켓 생성과 종료 | `socket()`, `close()`                                                       |  O   |
| 01-address         | 주소 체계와 변환 | `htons()`, `htonl()`, `ntohs()`, `ntohl()`,<br>`inet_pton()`, `inet_ntop()` |  O   |
| 02-dns             | 이름 해석        | `getaddrinfo()`, `getnameinfo()`, `freeaddrinfo()`                          |  O   |
| 03-01-tcp-basic    | TCP 통신         | `bind()`, `listen()`, `accept()`, `connect()`,<br>`send()`, `recv()`        |  O   |
| 04-01-udp-basic    | UDP 통신         | `sendto()`, `recvfrom()`                                                    |  O   |
| 05-socket-option   | 소켓 옵션        | `setsockopt()`, `getsockopt()`                                              |  O   |
| 03-02-tcp-echo     | TCP 에코 서버    |                                                                             |  O   |
| 04-02-udp-echo     | UDP 에코 서버    |                                                                             |      |
| 06-socket-error    | 소켓 에러 처리   |                                                                             |  O   |
| 07-signal          | 시그널 처리      | `signal()`, `sigaction()`                                                   |  O   |
| 08-nonblocking     | 논블로킹 소켓    | `fcntl()`, `ioctl()`                                                        |  O   |
| 09-summary         | 총망라           |                                                                             |      |
| 10-io-multiplexing | I/O 멀티플렉싱   |                                                                             |  O   |
| 11-select          | select           | `select()`, `FD_SET()`                                                      |      |
| 03-03-tcp-select   | TCP 채팅 서버    |                                                                             |      |
| 04-03-udp-select   | UDP 채팅 서버    |                                                                             |      |
| 12-poll            | poll             | `poll()`                                                                    |      |
| 13-epoll           | epoll            | `epoll_create()`, `epoll_ctl()`, `epoll_wait()`                             |  O   |

각 디렉토리는 `test/` 폴더에서 GTest로 함수와 구조체를 직접 사용해보는 방식으로 구성.
