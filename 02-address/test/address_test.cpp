#include <arpa/inet.h>
#include <netinet/in.h>

#include <gtest/gtest.h>

TEST(Address, SockaddrInSize) { EXPECT_EQ(sizeof(sockaddr_in), 16); }

TEST(Address, HtonsConvertsToNetworkByteOrder) {
    uint16_t host_port = 8080;
    uint16_t net_port = htons(host_port);
    EXPECT_EQ(ntohs(net_port), 8080);
}

TEST(Address, InetPtonConvertsIpv4String) {
    in_addr addr{};
    int result = inet_pton(AF_INET, "127.0.0.1", &addr);
    EXPECT_EQ(result, 1);
}

TEST(Address, InetNtopConvertsToString) {
    in_addr addr{};
    inet_pton(AF_INET, "192.168.1.1", &addr);

    char buf[INET_ADDRSTRLEN]{};
    const char* result = inet_ntop(AF_INET, &addr, buf, sizeof(buf));
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(buf, "192.168.1.1");
}

TEST(Address, InetPtonInvalidAddressFails) {
    in_addr addr{};
    int result = inet_pton(AF_INET, "invalid", &addr);
    EXPECT_EQ(result, 0);
}
