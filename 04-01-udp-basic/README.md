# UDP(User Datagram Protocol)

비연결형, 비신뢰성 데이터그램 통신을 제공하는 프로토콜. TCP와 달리 연결 수립 없이 바로 데이터를 보낸다.

## 흐름

```
서버                              클라이언트
──────                            ──────────
socket()                          socket()
bind()
recvfrom() ← 대기                 sendto() → 데이터 전송
  ← 데이터 수신
close()                           close()
```

TCP와 비교하면 `listen()`, `accept()`, `connect()` 과정이 없다.

## 함수

### `sendto()`

```cpp
#include <sys/socket.h>

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

지정한 주소로 데이터를 보낸다. 연결 없이 매번 목적지를 지정한다.

| 인수        | 설명                 |
| ----------- | -------------------- |
| `sockfd`    | 소켓 fd              |
| `buf`       | 보낼 데이터 버퍼     |
| `len`       | 보낼 바이트 수       |
| `flags`     | 전송 옵션 (보통 `0`) |
| `dest_addr` | 목적지 주소          |
| `addrlen`   | 주소 구조체 크기     |

#### 반환 값

| 결과 | 반환 값          | 비고         |
| ---- | ---------------- | ------------ |
| 성공 | 전송한 바이트 수 |              |
| 실패 | -1               | `errno` 설정 |

### `recvfrom()`

```cpp
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

데이터를 수신하고, 보낸 쪽의 주소를 함께 받는다.

| 인수       | 설명                                       |
| ---------- | ------------------------------------------ |
| `sockfd`   | 소켓 fd                                    |
| `buf`      | 데이터를 저장할 버퍼                       |
| `len`      | 버퍼 크기                                  |
| `flags`    | 수신 옵션 (보통 `0`)                       |
| `src_addr` | 송신자 주소를 저장할 구조체 (nullptr 가능) |
| `addrlen`  | 주소 구조체 크기 포인터 (nullptr 가능)     |

#### 반환 값

| 결과 | 반환 값          | 비고         |
| ---- | ---------------- | ------------ |
| 성공 | 수신한 바이트 수 |              |
| 실패 | -1               | `errno` 설정 |
