# I/O 멀티플렉싱(I/O Multiplexing)

하나의 스레드에서 여러 fd를 동시에 감시하는 기법. 블로킹 모델에서는 하나의 fd에서 대기하면 다른 fd를 처리할 수 없지만, I/O 멀티플렉싱은 "준비된 fd만 처리"할 수 있다.

## 왜 필요한가

### 블로킹 모델의 한계

```
클라이언트 A ──→ recv() 대기 중 ← 여기서 블록
클라이언트 B ──→ 처리 불가 (A에서 막혀있으니까)
클라이언트 C ──→ 처리 불가
```

### 해결 방법 비교

| 방식               | 설명                    | 단점                       |
| ------------------ | ----------------------- | -------------------------- |
| 멀티프로세스       | 클라이언트마다 `fork()` | 프로세스 생성 비용, 메모리 |
| 멀티스레드         | 클라이언트마다 스레드   | 스레드 생성 비용, 동기화   |
| 논블로킹 폴링      | 모든 fd를 반복하며 확인 | CPU 낭비 (busy-wait)       |
| **I/O 멀티플렉싱** | 준비된 fd만 알려줌      | 없음 (표준 방식)           |

### I/O 멀티플렉싱 모델

```
         ┌─── fd 3 (클라이언트 A)
         │
epoll ───┼─── fd 4 (클라이언트 B)    →  "fd 4 읽기 가능!"  →  recv(fd 4)
         │
         └─── fd 5 (클라이언트 C)
```

하나의 스레드에서 여러 fd를 감시하다가, 이벤트가 발생한 fd만 처리한다.

## Linux의 I/O 멀티플렉싱 API

| API        | 등장             | fd 제한           | 성능                              | 현재 상태 |
| ---------- | ---------------- | ----------------- | --------------------------------- | --------- |
| `select()` | 1983 (BSD)       | 1024 (FD_SETSIZE) | O(n) — 매번 전체 fd 전달          | 레거시    |
| `poll()`   | 1986 (SVR3)      | 없음              | O(n) — 매번 전체 fd 전달          | 레거시    |
| `epoll`    | 2002 (Linux 2.6) | 없음              | O(1) — 커널에 등록, 이벤트만 반환 | **표준**  |

### select

```cpp
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd, &readfds);

select(fd + 1, &readfds, nullptr, nullptr, nullptr);

if (FD_ISSET(fd, &readfds)) {
    // fd 읽기 가능
}
```

- 매번 fd_set을 커널에 복사
- 반환 후 어떤 fd가 준비됐는지 전체 순회 필요
- fd 1024개 제한

### poll

```cpp
pollfd fds[2];
fds[0].fd = fd1;
fds[0].events = POLLIN;
fds[1].fd = fd2;
fds[1].events = POLLIN;

poll(fds, 2, -1);

if (fds[0].revents & POLLIN) {
    // fd1 읽기 가능
}
```

- fd 개수 제한 없음
- 여전히 매번 전체 배열을 커널에 복사
- 반환 후 전체 순회 필요

### epoll

```cpp
int epfd = epoll_create1(0);

epoll_event ev{};
ev.events = EPOLLIN;
ev.data.fd = fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

epoll_event events[64];
int n = epoll_wait(epfd, events, 64, -1);

for (int i = 0; i < n; i++) {
    // events[i].data.fd 가 준비됨
}
```

- fd를 커널에 한 번만 등록 (`epoll_ctl`)
- 이벤트 발생한 fd만 반환 (`epoll_wait`)
- O(1) 성능

## select → poll → epoll 핵심 차이

```
select/poll:
  매 호출마다: [fd1, fd2, fd3, ..., fd1000] → 커널에 전달 → 전체 순회 → 결과 반환
                                                              O(n)

epoll:
  등록 (한 번): epoll_ctl(ADD, fd1), epoll_ctl(ADD, fd2), ...
  대기 (매번): epoll_wait() → 이벤트 발생한 fd만 반환
                                    O(1)
```

## 트리거 모드

epoll은 두 가지 트리거 모드를 지원한다.

| 모드                 | 플래그    | 동작                       |
| -------------------- | --------- | -------------------------- |
| Level-Triggered (LT) | 기본      | 데이터가 있는 한 계속 알림 |
| Edge-Triggered (ET)  | `EPOLLET` | 상태가 변할 때만 알림      |

```
데이터 100바이트 도착, 50바이트만 읽음:

LT: 다음 epoll_wait()에서 또 알림 (아직 50바이트 남아있으니까)
ET: 다음 epoll_wait()에서 알림 없음 (새 데이터가 와야 알림)
```

ET 모드는 반드시 **논블로킹 소켓 + EAGAIN까지 읽기**와 함께 사용해야 한다.
