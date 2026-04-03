# epoll 기반 TCP 브로드캐스트

싱글 스레드 + epoll 서버가 한 클라이언트의 메시지를 나머지 모든 클라이언트에게 전달한다.

## 에코 서버와의 차이

|                 | 에코 (14-01) | 브로드캐스트 (14-02)            |
| --------------- | ------------ | ------------------------------- |
| send 대상       | 보낸 사람    | 보낸 사람 제외 전원             |
| 클라이언트 관리 | 없음         | `unordered_set<int>`            |
| 클라이언트 동작 | stdin 입력   | 자동 하트비트 (1~5초 랜덤)      |
| id 부여         | 없음         | 서버가 accept 시 fd를 id로 전달 |

## 실행

```bash
# 서버
./build/14-02-tcp-broadcast/epoll_broadcast_server

# 클라이언트 (여러 개)
./build/14-02-tcp-broadcast/epoll_broadcast_client
./build/14-02-tcp-broadcast/epoll_broadcast_client
./build/14-02-tcp-broadcast/epoll_broadcast_client
```
