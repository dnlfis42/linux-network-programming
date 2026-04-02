#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <cstring>

#include <gtest/gtest.h>

TEST(Dns, GetaddrinfoResolvesLocalhost) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    int ret = getaddrinfo("localhost", "80", &hints, &result);
    ASSERT_EQ(ret, 0);
    ASSERT_NE(result, nullptr);

    auto* addr = reinterpret_cast<sockaddr_in*>(result->ai_addr);
    char ip[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
    EXPECT_STREQ(ip, "127.0.0.1");

    freeaddrinfo(result);
}

TEST(Dns, GetaddrinfoWithNullHost) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* result = nullptr;
    int ret = getaddrinfo(nullptr, "8080", &hints, &result);
    ASSERT_EQ(ret, 0);
    ASSERT_NE(result, nullptr);

    auto* addr = reinterpret_cast<sockaddr_in*>(result->ai_addr);
    EXPECT_EQ(addr->sin_addr.s_addr, htonl(INADDR_ANY));

    freeaddrinfo(result);
}

TEST(Dns, GetnameinfoReverseLookup) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char host[NI_MAXHOST]{};
    char serv[NI_MAXSERV]{};
    int ret = getnameinfo(
        reinterpret_cast<sockaddr*>(&addr), sizeof(addr), host, sizeof(host),
        serv, sizeof(serv), NI_NUMERICSERV
    );
    ASSERT_EQ(ret, 0);
    EXPECT_STREQ(serv, "80");
}

TEST(Dns, GetaddrinfoInvalidHostFails) {
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    int ret =
        getaddrinfo("this.host.does.not.exist.invalid", "80", &hints, &result);
    EXPECT_NE(ret, 0);
}

TEST(Dns, GaiStrerrorReturnsMessage) {
    const char* msg = gai_strerror(EAI_NONAME);
    EXPECT_NE(msg, nullptr);
    EXPECT_GT(strlen(msg), 0u);
}
