#if 0
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <netdb.h>
#include <linux/ethtool.h>
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#endif

#include <linux/filter.h>
#include <netinet/in.h>

#include "callback_test.h"
#include "util_debug.hpp"
#include "util_sock.hpp"

typedef struct {
    struct ethhdr head;
    char msg[32];
}jut_fc_cmd_t;


int raw_cli(int argc, char **argv)
{
    SockRaw raw(0x9090);

    raw.open();
    raw.bind_to_device(argv[1]);
    bool flag = atoi(argv[2]);

    jut_fc_cmd_t cmd;
    unsigned char src_mac[6] = {0xaa, 0xbb, 0xcc};
    unsigned char dst_mac1[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char dst_mac2[6] = {0};

    memset(&cmd, 0, sizeof(cmd));
    if (flag) {
        memcpy(cmd.head.h_dest, dst_mac1, 6);
    } else {
        memcpy(cmd.head.h_dest, dst_mac2, 6);
    }
    memcpy(cmd.head.h_source, src_mac, 6);
    cmd.head.h_proto = htons(0x9090);
    raw.sendn(&cmd, sizeof(cmd));
    return 0;
}

int raw_svr(int argc, char **argv)
{
    INFO("enter");
    while (1) {
        SockRaw raw(ETH_P_ALL);

        if (!raw.open()) {
            ERR("open");
            sleep(1);
            continue;
        }
        raw.set_eth_filter(0x9090);
        int recv_count = 0;
        while (1) {
            int ret = raw.poll(5);
            if (ret < 0) {
                ERR("poll %s", strerror(errno));
                break;
            } else if (ret == 0) {
                INFO("no data");
            } else {
                unsigned char buf[1514];
                memset(buf, 0, sizeof(buf));
                int ret = raw.recv(buf, sizeof(buf));
                if (ret <= 0) {
                    ERR("recv %s", strerror(errno));
                    break;
                } else {
//                    jut_fc_cmd_t *head = reinterpret_cast<jut_fc_cmd_t*>(buf);
                    int n;
                    for (n = 0; n < ret; n++) {
                        recv_count++;
                        printf("%02x ", buf[n]);
                        fflush(stdout);
                    }
                    printf("\n");
                }
            }
        }
    }
    return 0;
}

TEST_REG(raw)
{
    INFO("enter");


    return 0;
}
#endif
