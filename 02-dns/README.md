# DNS(Domain Name System)

도메인 이름과 IP 주소를 상호 변환하는 체계.

## 헤더

- `<netdb.h>` — `getaddrinfo()`, `getnameinfo()`, `freeaddrinfo()`, `gai_strerror()`

## 구조체

### `addrinfo`

```cpp
#include <netdb.h>

struct addrinfo {
    int              ai_flags;      // AI_PASSIVE, AI_CANONNAME 등
    int              ai_family;     // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;   // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;   // 프로토콜 번호
    socklen_t        ai_addrlen;    // ai_addr 크기
    struct sockaddr *ai_addr;       // 소켓 주소
    char            *ai_canonname;  // 정식 호스트 이름
    struct addrinfo *ai_next;       // 다음 결과 (연결 리스트)
};
```

## 함수

### `getaddrinfo()`

```cpp
#include <netdb.h>

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints, struct addrinfo **res);
```

호스트 이름과 서비스 이름을 소켓 주소로 변환한다.

| 인수      | 설명                                         |
| --------- | -------------------------------------------- |
| `node`    | 호스트 이름 또는 IP 문자열 (nullptr 가능)    |
| `service` | 서비스 이름 또는 포트 문자열 (nullptr 가능)  |
| `hints`   | 결과 필터링 조건                             |
| `res`     | 결과를 저장할 연결 리스트 포인터             |

#### 반환 값

| 결과 | 반환 값         | 비고                        |
| ---- | --------------- | --------------------------- |
| 성공 | 0               |                             |
| 실패 | 0이 아닌 에러코드 | `gai_strerror()`로 확인    |

### `freeaddrinfo()`

```cpp
#include <netdb.h>

void freeaddrinfo(struct addrinfo *res);
```

`getaddrinfo()`가 할당한 연결 리스트를 해제한다.

### `getnameinfo()`

```cpp
#include <netdb.h>

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags);
```

소켓 주소를 호스트 이름과 서비스 이름으로 역변환한다.

| 인수      | 설명                                    |
| --------- | --------------------------------------- |
| `addr`    | 변환할 소켓 주소                        |
| `addrlen` | 주소 구조체 크기                        |
| `host`    | 호스트 이름을 저장할 버퍼 (nullptr 가능) |
| `hostlen` | 호스트 버퍼 크기                        |
| `serv`    | 서비스 이름을 저장할 버퍼 (nullptr 가능) |
| `servlen` | 서비스 버퍼 크기                        |
| `flags`   | 변환 옵션                               |

#### 주요 flags

| 플래그          | 설명                              |
| --------------- | --------------------------------- |
| `NI_NUMERICHOST` | 호스트를 숫자(IP) 형태로 반환    |
| `NI_NUMERICSERV` | 서비스를 숫자(포트) 형태로 반환  |

#### 반환 값

| 결과 | 반환 값           | 비고                       |
| ---- | ----------------- | -------------------------- |
| 성공 | 0                 |                            |
| 실패 | 0이 아닌 에러코드 | `gai_strerror()`로 확인   |

### `gai_strerror()`

```cpp
#include <netdb.h>

const char *gai_strerror(int errcode);
```

`getaddrinfo()`/`getnameinfo()`의 에러코드를 문자열로 변환한다. DNS 함수들은 `errno`를 쓰지 않고 자체 에러코드를 반환하므로 `strerror()` 대신 이 함수를 사용한다.
