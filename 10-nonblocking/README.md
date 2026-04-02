# 논블로킹 소켓(Nonblocking Socket)

블로킹 소켓은 데이터가 올 때까지 대기하지만, 논블로킹 소켓은 즉시 반환한다.

## 블로킹 vs 논블로킹

|                      | 블로킹 (기본)         | 논블로킹                     |
| -------------------- | --------------------- | ---------------------------- |
| 데이터 없이 `recv()` | 데이터 올 때까지 대기 | 즉시 -1 반환 (`EAGAIN`)      |
| 연결 없이 `accept()` | 연결 올 때까지 대기   | 즉시 -1 반환 (`EAGAIN`)      |
| `connect()`          | 연결 완료까지 대기    | 즉시 -1 반환 (`EINPROGRESS`) |

## 설정 방법

### 방법 1: `fcntl()`

```cpp
#include <fcntl.h>

int flags = fcntl(fd, F_GETFL, 0);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

### 방법 2: `socket()` 생성 시

```cpp
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
```

## 함수

### `fcntl()`

```cpp
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
```

파일 디스크립터의 속성을 조회/변경한다.

| cmd       | 설명                              |
| --------- | --------------------------------- |
| `F_GETFL` | 현재 플래그 조회                  |
| `F_SETFL` | 플래그 설정                       |
| `F_GETFD` | fd 플래그 조회 (close-on-exec 등) |
| `F_SETFD` | fd 플래그 설정                    |

## 논블로킹에서의 에러코드

| 에러코드                 | 함수                           | 의미                                 |
| ------------------------ | ------------------------------ | ------------------------------------ |
| `EAGAIN` / `EWOULDBLOCK` | `recv()`, `send()`, `accept()` | 지금은 불가, 나중에 재시도           |
| `EINPROGRESS`            | `connect()`                    | 연결 진행 중, `SO_ERROR`로 결과 확인 |

## 논블로킹 connect 패턴

```cpp
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

int ret = connect(fd, addr, addrlen);
if (ret == -1 && errno == EINPROGRESS) {
    // 연결 진행 중 — epoll/select로 쓰기 가능 대기
    // 완료 후 SO_ERROR로 결과 확인
    int err = 0;
    socklen_t len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (err == 0) {
        // 연결 성공
    }
}
```

논블로킹 소켓은 단독으로 쓰면 busy-wait(계속 재시도)이 되므로, 보통 `epoll`과 함께 사용한다.
