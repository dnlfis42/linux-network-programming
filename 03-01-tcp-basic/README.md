# TCP(Transmission Control Protocol)

연결 지향적이고 신뢰성 있는 바이트 스트림 통신을 제공하는 프로토콜.

## 흐름

```
서버                              클라이언트
──────                            ──────────
socket()                          socket()
bind()
listen()
accept() ← 대기                   connect() → 연결 요청
  ← 연결 수립                       ↓
recv() ← 대기                     send() → 데이터 전송
  ← 데이터 수신                     ↓
close()                           close()
```

## 함수

### `bind()`

```cpp
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

소켓에 주소(IP + 포트)를 할당한다.

| 인수      | 설명                                    |
| --------- | --------------------------------------- |
| `sockfd`  | 소켓 fd                                 |
| `addr`    | 바인딩할 주소 (`sockaddr_in*`를 캐스팅) |
| `addrlen` | 주소 구조체 크기                        |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

### `listen()`

```cpp
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

소켓을 연결 대기 상태로 전환한다.

| 인수      | 설명                               |
| --------- | ---------------------------------- |
| `sockfd`  | 소켓 fd                            |
| `backlog` | 대기 큐 크기 (완료된 연결 대기 수) |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

### `accept()`

```cpp
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

대기 큐에서 연결을 꺼내 새로운 소켓 fd를 반환한다. 큐가 비어있으면 블록된다.

| 인수      | 설명                                           |
| --------- | ---------------------------------------------- |
| `sockfd`  | listen 중인 소켓 fd                            |
| `addr`    | 클라이언트 주소를 저장할 구조체 (nullptr 가능) |
| `addrlen` | 주소 구조체 크기 포인터 (nullptr 가능)         |

#### 반환 값

| 결과 | 반환 값      | 비고         |
| ---- | ------------ | ------------ |
| 성공 | 새 연결의 fd |              |
| 실패 | -1           | `errno` 설정 |

### `connect()`

```cpp
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

서버에 연결을 요청한다.

| 인수      | 설명             |
| --------- | ---------------- |
| `sockfd`  | 소켓 fd          |
| `addr`    | 서버 주소        |
| `addrlen` | 주소 구조체 크기 |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

### `send()`

```cpp
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

연결된 소켓으로 데이터를 보낸다.

| 인수     | 설명                 |
| -------- | -------------------- |
| `sockfd` | 소켓 fd              |
| `buf`    | 보낼 데이터 버퍼     |
| `len`    | 보낼 바이트 수       |
| `flags`  | 전송 옵션 (보통 `0`) |

#### 반환 값

| 결과 | 반환 값          | 비고         |
| ---- | ---------------- | ------------ |
| 성공 | 전송한 바이트 수 |              |
| 실패 | -1               | `errno` 설정 |

### `recv()`

```cpp
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

연결된 소켓에서 데이터를 받는다.

| 인수     | 설명                 |
| -------- | -------------------- |
| `sockfd` | 소켓 fd              |
| `buf`    | 데이터를 저장할 버퍼 |
| `len`    | 버퍼 크기            |
| `flags`  | 수신 옵션 (보통 `0`) |

#### 반환 값

| 결과      | 반환 값          | 비고             |
| --------- | ---------------- | ---------------- |
| 성공      | 수신한 바이트 수 |                  |
| 연결 종료 | 0                | 상대가 close()함 |
| 실패      | -1               | `errno` 설정     |

### `setsockopt()`

```cpp
#include <sys/socket.h>

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

소켓 옵션을 설정한다.

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

### `getsockname()`

```cpp
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

소켓에 바인딩된 주소를 가져온다. 포트를 `0`으로 바인딩한 경우 커널이 할당한 실제 포트를 확인할 때 사용한다.

| 인수      | 설명                    |
| --------- | ----------------------- |
| `sockfd`  | 소켓 fd                 |
| `addr`    | 주소를 저장할 구조체    |
| `addrlen` | 주소 구조체 크기 포인터 |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |
