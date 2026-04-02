# 시그널(Signal)

프로세스에게 비동기적으로 이벤트를 알리는 메커니즘. 소켓 프로그래밍에서 연결 종료, 프로세스 종료 등을 처리할 때 사용한다.

## 주요 시그널

| 시그널    | 번호 | 기본 동작 | 발생 상황                    |
| --------- | ---- | --------- | ---------------------------- |
| `SIGINT`  | 2    | 종료      | Ctrl+C                       |
| `SIGPIPE` | 13   | 종료      | 닫힌 연결에 `send()`         |
| `SIGTERM` | 15   | 종료      | `kill` 명령 (기본)           |
| `SIGKILL` | 9    | 종료      | 강제 종료 (무시/핸들링 불가) |
| `SIGCHLD` | 17   | 무시      | 자식 프로세스 종료           |
| `SIGUSR1` | 10   | 종료      | 사용자 정의                  |
| `SIGUSR2` | 12   | 종료      | 사용자 정의                  |

## 함수

### `signal()`

```cpp
#include <csignal>

sighandler_t signal(int signum, sighandler_t handler);
```

시그널 핸들러를 등록한다. 간단하지만 이식성과 세밀한 제어가 부족해 `sigaction()`이 권장된다.

| 인수      | 설명                                                 |
| --------- | ---------------------------------------------------- |
| `signum`  | 시그널 번호                                          |
| `handler` | `SIG_IGN` (무시), `SIG_DFL` (기본), 또는 핸들러 함수 |

#### 반환 값

| 결과 | 반환 값     | 비고 |
| ---- | ----------- | ---- |
| 성공 | 이전 핸들러 |      |
| 실패 | `SIG_ERR`   |      |

### `sigaction()`

```cpp
#include <csignal>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

시그널 핸들러를 등록한다. `signal()`보다 세밀한 제어가 가능하다.

```cpp
struct sigaction {
    void     (*sa_handler)(int);        // 핸들러 함수
    sigset_t   sa_mask;                 // 핸들러 실행 중 블록할 시그널
    int        sa_flags;                // 옵션 플래그
};
```

| 인수     | 설명                                      |
| -------- | ----------------------------------------- |
| `signum` | 시그널 번호                               |
| `act`    | 새 핸들러 설정 (nullptr이면 변경 안 함)   |
| `oldact` | 이전 핸들러 저장 (nullptr이면 저장 안 함) |

#### 주요 플래그

| 플래그         | 설명                                         |
| -------------- | -------------------------------------------- |
| `SA_RESTART`   | 시그널에 의해 중단된 시스템 콜을 자동 재시작 |
| `SA_NOCLDSTOP` | 자식 프로세스 정지 시 `SIGCHLD` 발생 안 함   |
| `SA_SIGINFO`   | 확장 핸들러 `sa_sigaction` 사용              |

## 소켓에서의 시그널

### SIGPIPE 방지

닫힌 연결에 `send()`하면 `SIGPIPE`로 프로세스가 죽는다. 두 가지 방법으로 방지:

```cpp
// 방법 1: 전역 무시
signal(SIGPIPE, SIG_IGN);

// 방법 2: send()마다 플래그 지정
send(fd, buf, len, MSG_NOSIGNAL);
```

### EINTR — 시그널에 의한 시스템 콜 중단

`accept()`, `recv()` 등 블로킹 시스템 콜이 시그널에 의해 중단되면 `-1`을 반환하고 `errno`가 `EINTR`로 설정된다. `SA_RESTART`를 설정하면 자동으로 재시작된다.
