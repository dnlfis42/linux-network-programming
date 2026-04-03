# epoll 기반 TCP 에코 서버

싱글 스레드 + epoll(LT 모드)로 여러 클라이언트를 동시에 처리하는 에코 서버.

## 구조

```
서버 (epoll, 논블로킹)                 클라이언트 (블로킹)
──────────────────────                 ──────────────────
socket(SOCK_NONBLOCK)                  socket()
bind()                                 connect()
listen()                               ↓
epoll_create1()                        send() → 서버
epoll_ctl(ADD, listen_fd)              recv() ← 서버
↓                                      ↓
epoll_wait()                           반복
├── listen_fd: accept4() → epoll 등록
├── EPOLLIN:   recv() → send() (에코)
└── EPOLLRDHUP: close() → epoll 제거
```

## 실행

```bash
# 서버
./build/14-01-tcp-echo/epoll_echo_server

# 클라이언트 (여러 개 동시 가능)
./build/14-01-tcp-echo/epoll_echo_client
```

## 사용 기술

- epoll LT 모드
- 논블로킹 소켓 (`SOCK_NONBLOCK`, `accept4()`)
- `EPOLLRDHUP` — 클라이언트 종료 감지
- `SIGPIPE` 무시
