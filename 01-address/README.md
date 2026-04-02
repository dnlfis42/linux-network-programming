# 주소(Address)

소켓 통신에서 목적지와 출발지를 지정하기 위한 주소 체계와 변환 함수.

## 헤더

- `<netinet/in.h>` — 주소 구조체, 바이트 순서 변환
- `<arpa/inet.h>` — IP 주소 문자열 변환

## 구조체

### `sockaddr`

프로토콜에 독립적인 범용 주소 구조체. `bind()`, `connect()` 등의 함수에서 인자 타입으로 사용된다. 실제로는 `sockaddr_in`을 캐스팅해서 전달한다.

```cpp
#include <sys/socket.h>

struct sockaddr {
    sa_family_t sa_family;  // 주소 패밀리
    char        sa_data[14];
};
```

### `sockaddr_in`

IPv4 소켓 주소 구조체. 크기는 16바이트.

```cpp
#include <netinet/in.h>

struct sockaddr_in {
    sa_family_t    sin_family;  // 주소 패밀리 (AF_INET)
    in_port_t      sin_port;    // 포트 번호 (네트워크 바이트 순서)
    struct in_addr sin_addr;    // IPv4 주소 (네트워크 바이트 순서)
};
```

## 함수

### `htons()` / `ntohs()`

```cpp
#include <netinet/in.h>

uint16_t htons(uint16_t hostshort);   // host → network (16비트)
uint16_t ntohs(uint16_t netshort);    // network → host (16비트)
```

호스트 바이트 순서와 네트워크 바이트 순서(빅 엔디안) 간 변환.
포트 번호를 `sockaddr_in.sin_port`에 설정할 때 사용한다.

### `inet_pton()`

```cpp
#include <arpa/inet.h>

int inet_pton(int af, const char *restrict src, void *restrict dst);
```

IP 주소 문자열을 이진 형태로 변환한다.

| 인수  | 설명                                |
| ----- | ----------------------------------- |
| `af`  | 주소 패밀리 (`AF_INET`, `AF_INET6`) |
| `src` | IP 주소 문자열 (`"127.0.0.1"`)      |
| `dst` | 변환 결과를 저장할 구조체 포인터    |

#### 반환 값

| 결과          | 반환 값 | 비고         |
| ------------- | ------- | ------------ |
| 성공          | 1       |              |
| 잘못된 주소   | 0       |              |
| 잘못된 패밀리 | -1      | `errno` 설정 |

### `inet_ntop()`

```cpp
#include <arpa/inet.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

이진 형태의 IP 주소를 문자열로 변환한다.

| 인수   | 설명                                |
| ------ | ----------------------------------- |
| `af`   | 주소 패밀리 (`AF_INET`, `AF_INET6`) |
| `src`  | 이진 주소 구조체 포인터             |
| `dst`  | 문자열을 저장할 버퍼                |
| `size` | 버퍼 크기 (`INET_ADDRSTRLEN` = 16)  |

#### 반환 값

| 결과 | 반환 값      | 비고         |
| ---- | ------------ | ------------ |
| 성공 | `dst` 포인터 |              |
| 실패 | `nullptr`    | `errno` 설정 |

## 매크로

### `INET_ADDRSTRLEN`

IPv4 주소를 담을 수 있는 버퍼의 길이

```cpp
#include <netinet/in.h>

#define INET_ADDRSTRLEN 16
```
