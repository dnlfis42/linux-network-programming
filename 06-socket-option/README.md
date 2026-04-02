# 소켓 옵션(Socket Option)

`setsockopt()`/`getsockopt()`로 소켓의 동작을 제어한다.

## 함수

### `setsockopt()`

```cpp
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
```

| 인수      | 설명                          |
| --------- | ----------------------------- |
| `sockfd`  | 소켓 fd                       |
| `level`   | 옵션 레벨 (`SOL_SOCKET` 등)   |
| `optname` | 옵션 이름 (`SO_REUSEADDR` 등) |
| `optval`  | 옵션 값 포인터                |
| `optlen`  | 옵션 값 크기                  |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

### `getsockopt()`

```cpp
#include <sys/socket.h>

int getsockopt(int sockfd, int level, int optname,
               void *optval, socklen_t *optlen);
```

| 인수      | 설명               |
| --------- | ------------------ |
| `sockfd`  | 소켓 fd            |
| `level`   | 옵션 레벨          |
| `optname` | 옵션 이름          |
| `optval`  | 값을 저장할 포인터 |
| `optlen`  | 값 크기 포인터     |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

## 옵션 목록 (SOL_SOCKET)

### 실용적 옵션

| 옵션           | 값 타입          | 설명                                                           |
| -------------- | ---------------- | -------------------------------------------------------------- |
| `SO_REUSEADDR` | `int` (bool)     | 주소 재사용 허용. 서버 재시작 시 "Address already in use" 방지 |
| `SO_REUSEPORT` | `int` (bool)     | 같은 주소+포트에 여러 소켓 바인딩 허용                         |
| `SO_KEEPALIVE` | `int` (bool)     | TCP 연결 유지 확인 메시지를 주기적으로 전송                    |
| `SO_LINGER`    | `struct linger`  | `close()` 시 미전송 데이터 처리 방식 제어                      |
| `SO_RCVBUF`    | `int`            | 수신 버퍼 크기. 커널이 설정값의 2배로 할당                     |
| `SO_SNDBUF`    | `int`            | 송신 버퍼 크기. 커널이 설정값의 2배로 할당                     |
| `SO_RCVTIMEO`  | `struct timeval` | 수신 타임아웃. 0이면 무한 대기                                 |
| `SO_SNDTIMEO`  | `struct timeval` | 송신 타임아웃. 0이면 무한 대기                                 |
| `SO_BROADCAST` | `int` (bool)     | UDP 브로드캐스트 허용                                          |

### 읽기 전용 옵션

| 옵션            | 값 타입 | 설명                                    |
| --------------- | ------- | --------------------------------------- |
| `SO_ACCEPTCONN` | `int`   | listen 상태 여부 (0 또는 1)             |
| `SO_TYPE`       | `int`   | 소켓 타입 (`SOCK_STREAM`, `SOCK_DGRAM`) |
| `SO_DOMAIN`     | `int`   | 소켓 도메인 (`AF_INET` 등)              |
| `SO_PROTOCOL`   | `int`   | 프로토콜 (`IPPROTO_TCP` 등)             |
| `SO_ERROR`      | `int`   | 대기 중인 에러 조회 후 클리어           |

### 기타 옵션 (참고)

| 옵션                                 | 설명                                          |
| ------------------------------------ | --------------------------------------------- |
| `SO_ATTACH_FILTER` / `SO_ATTACH_BPF` | BPF 패킷 필터 설정                            |
| `SO_BINDTODEVICE`                    | 특정 네트워크 인터페이스에 바인딩 (root 필요) |
| `SO_MARK`                            | 패킷 마킹 (root 필요)                         |
| `SO_DEBUG`                           | 소켓 디버깅 (root 필요)                       |
| `SO_TIMESTAMP` / `SO_TIMESTAMPNS`    | 패킷 수신 타임스탬프                          |
| `SO_BUSY_POLL`                       | 저지연 busy polling (root 필요)               |
| `SO_INCOMING_CPU`                    | CPU 친화성 설정                               |
| `SO_PASSCRED` / `SO_PEERCRED`        | UNIX 도메인 소켓 자격증명                     |
| `SO_OOBINLINE`                       | OOB 데이터를 일반 수신 스트림에 포함          |
| `SO_PRIORITY`                        | 패킷 우선순위 (root 필요)                     |
| `SO_RCVBUFFORCE` / `SO_SNDBUFFORCE`  | 버퍼 크기 강제 설정 (root 필요)               |
| `SO_RCVLOWAT` / `SO_SNDLOWAT`        | 수신/송신 최소 바이트 수                      |
| `SO_BSDCOMPAT`                       | deprecated                                    |
