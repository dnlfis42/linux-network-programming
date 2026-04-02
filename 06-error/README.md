# 소켓 에러(Socket Error)

소켓 함수 실패 시 에러를 확인하고 처리하는 방법.

## 에러 확인 함수

### `strerror()`

```cpp
#include <cstring>

char *strerror(int errnum);
```

`errno` 값을 사람이 읽을 수 있는 문자열로 변환한다.

### `perror()`

```cpp
#include <cstdio>

void perror(const char *s);
```

`s: 에러 메시지` 형태로 stderr에 출력한다.

### `gai_strerror()`

```cpp
#include <netdb.h>

const char *gai_strerror(int errcode);
```

`getaddrinfo()`/`getnameinfo()` 전용. 이 함수들은 `errno`를 쓰지 않고 자체 에러코드를 반환한다.

## 주요 에러코드

### 소켓 생성/설정

| 에러코드       | 값  | 발생 상황                 |
| -------------- | --- | ------------------------- |
| `EAFNOSUPPORT` |     | 지원하지 않는 주소 패밀리 |
| `EINVAL`       |     | 잘못된 인자               |

### 바인딩

| 에러코드     | 값  | 발생 상황                             |
| ------------ | --- | ------------------------------------- |
| `EADDRINUSE` | 98  | 주소(포트)가 이미 사용 중             |
| `EACCES`     | 13  | 권한 없음 (1024 미만 포트, root 필요) |

### 연결

| 에러코드       | 값  | 발생 상황                                   |
| -------------- | --- | ------------------------------------------- |
| `ECONNREFUSED` | 111 | 상대방이 연결을 거부 (포트가 열려있지 않음) |
| `ETIMEDOUT`    | 110 | 연결 시간 초과                              |
| `ENETUNREACH`  | 101 | 네트워크에 도달 불가                        |

### 송수신

| 에러코드                 | 값  | 발생 상황                         |
| ------------------------ | --- | --------------------------------- |
| `EBADF`                  | 9   | 잘못된 파일 디스크립터            |
| `EPIPE`                  | 32  | 상대방이 연결을 닫은 후 송신 시도 |
| `ECONNRESET`             | 104 | 상대방이 연결을 강제 종료 (RST)   |
| `EAGAIN` / `EWOULDBLOCK` | 11  | 논블로킹 모드에서 데이터 없음     |

### listen

| 에러코드     | 값  | 발생 상황                |
| ------------ | --- | ------------------------ |
| `EOPNOTSUPP` | 95  | UDP 소켓에서 listen 시도 |

## SO_ERROR

`getsockopt()`로 소켓에 대기 중인 에러를 확인하고 클리어한다.

```cpp
int err = 0;
socklen_t len = sizeof(err);
getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
// err != 0이면 에러 발생
```

논블로킹 `connect()` 후 연결 성공/실패를 확인할 때 주로 사용한다.
