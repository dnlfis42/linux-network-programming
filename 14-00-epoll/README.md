# epoll

Linux 전용 I/O 멀티플렉싱 API. 커널에 fd를 등록해두고, 이벤트가 발생한 fd만 반환받는다.

## 헤더

- `<sys/epoll.h>` — `epoll_create1()`, `epoll_ctl()`, `epoll_wait()`

## 구조체

### `epoll_event`

```cpp
#include <sys/epoll.h>

struct epoll_event {
    uint32_t     events;  // 이벤트 플래그
    epoll_data_t data;    // 사용자 데이터
};

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;
```

## 함수

### `epoll_create1()`

```cpp
#include <sys/epoll.h>

int epoll_create1(int flags);
```

epoll 인스턴스를 생성하고 fd를 반환한다.

| 인수    | 설명                     |
| ------- | ------------------------ |
| `flags` | `0` 또는 `EPOLL_CLOEXEC` |

#### 반환 값

| 결과 | 반환 값  | 비고         |
| ---- | -------- | ------------ |
| 성공 | epoll fd |              |
| 실패 | -1       | `errno` 설정 |

### `epoll_ctl()`

```cpp
#include <sys/epoll.h>

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

epoll에 fd를 등록, 수정, 삭제한다.

| 인수    | 설명                                              |
| ------- | ------------------------------------------------- |
| `epfd`  | epoll fd                                          |
| `op`    | `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, `EPOLL_CTL_DEL` |
| `fd`    | 대상 fd                                           |
| `event` | 이벤트 설정 (DEL 시 nullptr 가능)                 |

#### 반환 값

| 결과 | 반환 값 | 비고         |
| ---- | ------- | ------------ |
| 성공 | 0       |              |
| 실패 | -1      | `errno` 설정 |

### `epoll_wait()`

```cpp
#include <sys/epoll.h>

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
```

이벤트가 발생한 fd들을 반환한다.

| 인수        | 설명                                              |
| ----------- | ------------------------------------------------- |
| `epfd`      | epoll fd                                          |
| `events`    | 이벤트를 저장할 배열                              |
| `maxevents` | 배열 크기                                         |
| `timeout`   | 대기 시간 (ms). -1이면 무한 대기, 0이면 즉시 반환 |

#### 반환 값

| 결과        | 반환 값     | 비고         |
| ----------- | ----------- | ------------ |
| 이벤트 발생 | 이벤트 개수 |              |
| 타임아웃    | 0           |              |
| 실패        | -1          | `errno` 설정 |

## 이벤트 플래그

| 플래그         | 설명                                   |
| -------------- | -------------------------------------- |
| `EPOLLIN`      | 읽기 가능 (데이터 도착, 새 연결)       |
| `EPOLLOUT`     | 쓰기 가능 (송신 버퍼 여유 있음)        |
| `EPOLLERR`     | 에러 발생 (항상 감시됨, 등록 불필요)   |
| `EPOLLHUP`     | hang up (항상 감시됨)                  |
| `EPOLLRDHUP`   | 상대방이 연결을 닫음 (half-close 감지) |
| `EPOLLET`      | Edge-Triggered 모드                    |
| `EPOLLONESHOT` | 한 번 알린 후 비활성화 (재등록 필요)   |

## 트리거 모드

### Level-Triggered (LT, 기본)

데이터가 있는 한 `epoll_wait()`가 계속 알린다.

```cpp
ev.events = EPOLLIN;  // LT
```

### Edge-Triggered (ET)

상태가 변할 때만 알린다. 반드시 **논블로킹 소켓 + EAGAIN까지 읽기**와 함께 사용해야 한다.

```cpp
ev.events = EPOLLIN | EPOLLET;  // ET

// ET에서 읽기 패턴
while (true) {
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n == -1) {
        if (errno == EAGAIN) break;  // 다 읽었음
        // 에러 처리
    }
    if (n == 0) break;  // 연결 종료
    // 데이터 처리
}
```

## epoll 서버 기본 패턴

```cpp
int epfd = epoll_create1(0);

// listen 소켓 등록
epoll_event ev{};
ev.events = EPOLLIN;
ev.data.fd = listen_fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

epoll_event events[64];
while (running) {
    int n = epoll_wait(epfd, events, 64, -1);
    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == listen_fd) {
            // 새 연결 → accept → epoll에 등록
        } else {
            // 데이터 도착 → recv → 처리 → send
        }
    }
}
```
