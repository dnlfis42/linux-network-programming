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
ctest --test-dir build/01_socket/test

# 상세 출력
ctest --test-dir build/01_socket/test -V
```

## 구조

| 디렉토리   | 주제                                                 | 핵심 헤더                         |
| ---------- | ---------------------------------------------------- | --------------------------------- |
| 01_socket  | 소켓 생성과 종료                                     | `<sys/socket.h>`, `<unistd.h>`    |
| 02_address | 주소 체계와 변환                                     | `<netinet/in.h>`, `<arpa/inet.h>` |
| 03_tcp     | TCP 연결 (bind, listen, accept, connect, send, recv) | 위 전부                           |
| 04_udp     | UDP 통신 (sendto, recvfrom)                          | 위 전부                           |
| 05_dns     | 이름 해석 (getaddrinfo, getnameinfo)                 | `<netdb.h>`                       |
| 06_http    | HTTP 응용                                            | 위 전부                           |
| 09_summary | 총망라                                               | 전체                              |

각 디렉토리는 `test/` 폴더에서 GTest로 함수와 구조체를 직접 사용해보는 방식으로 구성.
