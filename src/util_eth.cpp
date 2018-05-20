#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
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
#include <list>
#include <string>
#include "util_eth.hpp"
#include "util_debug.hpp"
#include "util_file.hpp"

#define ETH_DBG(...) DBG("Eth", __VA_ARGS__)
#define ETH_INF(...) INF("Eth", __VA_ARGS__)
#define ETH_ERR(...) ERR("Eth", __VA_ARGS__)
#define ETH_WRN(...) WRN("Eth", __VA_ARGS__)
#define ETH_FTL(...) FTL("Eth", __VA_ARGS__)

namespace tiny_utils {
bool is_loopback(const string &ip)
{
    return ip == "lo";
}

bool get_netdev_index(int fd, const string &netdev, int &idx)
{
    struct ifreq if_req;

    memset(&if_req, 0, sizeof(if_req));
    snprintf(if_req.ifr_name, sizeof(if_req.ifr_name), "%s", netdev.c_str());

    if (ioctl(fd, SIOCGIFINDEX, &if_req) == -1) {
        ETH_ERR("SIOCGIFINDEXfailed. %s", strerror(errno));
        return false;
    }
    idx = if_req.ifr_ifindex;
    return true;
}

bool get_netdev_ipaddr(int fd, const string &netdev, string &ip)
{
    struct ifreq if_req;

    memset(&if_req, 0, sizeof(if_req));
    snprintf(if_req.ifr_name, sizeof(if_req.ifr_name), "%s", netdev.c_str());
    if (ioctl(fd, SIOCGIFADDR, &if_req) == 0) {
        char buf[24] = {0};
        struct sockaddr_in *sin;
        sin = (struct sockaddr_in*) &if_req.ifr_addr;
        inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
        ip = buf;
        return true;
    }
    return false;
}

bool get_netdev_netmask(int fd, const string &netdev, string &netmask)
{
    struct ifreq if_req;

    memset(&if_req, 0, sizeof(if_req));
    snprintf(if_req.ifr_name, sizeof(if_req.ifr_name), "%s", netdev.c_str());
    if (ioctl(fd, SIOCGIFNETMASK, &if_req) == 0) {
        char buf[24] = {0};
        struct sockaddr_in *sin;
        sin = (struct sockaddr_in*) &if_req.ifr_addr;
        inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf));
        netmask = buf;
        return true;
    }
    return false;
}

bool get_netdev_hwaddr(int fd, const string &netdev, unsigned char hwaddr[6])
{
    struct ifreq if_req;

    memset(&if_req, 0, sizeof(if_req));
    snprintf(if_req.ifr_name, sizeof(if_req.ifr_name), "%s", netdev.c_str());

    if (ioctl(fd, SIOCGIFHWADDR, &if_req) == 0) {
        memcpy(hwaddr, if_req.ifr_hwaddr.sa_data, 6);
        return true;
    }
    return false;
}

bool get_netdev_namelist(list<string> &lst)
{
    if (!CDir::scan("/sys/class/net", lst)) {
        lst.clear();
        return false;
    }

    return true;
}

bool get_netdev_speed(int fd, const string &netdev, int &speed)
{
    struct ethtool_cmd ep;
    struct ifreq if_req;

    memset(&ep, 0, sizeof(ep));
    memset(&if_req, 0, sizeof(if_req));

#ifndef SIOCETHTOOL
#define SIOCETHTOOL     0x8946
#endif

    ep.cmd = ETHTOOL_GSET;
    if_req.ifr_data = reinterpret_cast<char*>(&ep);
    snprintf(if_req.ifr_name, sizeof(if_req.ifr_name), "%s", netdev.c_str());
    if (ioctl(fd, SIOCETHTOOL, &if_req) == 0) {
        speed = ethtool_cmd_speed(&ep);
        return true;
    }

    return false;
}

bool get_netdev_driver(const string &ifname, string &drv_name)
{
    char buf[128];

    snprintf(buf, sizeof(buf), "/sys/class/net/%s/device/uevent", ifname.c_str());

    FILE *fp = NULL;
    fp = fopen(buf, "r");
    if (fp == NULL) {
        ETH_ERR("open. %s %s\n", buf, strerror(errno));
        return false;
    }

    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
        const char *p = strstr(buf, "DRIVER");
        if (p) {
            p = strchr(p, '=');
            if (p) {
                p++;
                if (*p != '\n' && *p != 0) {
                    drv_name = p;
                    fclose(fp);
                    return true;
                }
            }
            break;
        }
        memset(buf, 0, sizeof(buf));
    }
    fclose(fp);
    return false;
}

bool set_netdev_promiscuos(int fd, const string &ifname)
{
    struct ifreq ethreq;

    if (ifname.empty()) {
        return false;
    }

    /* Set the network card in promiscuos mode */
    memset(&ethreq, 0, sizeof(struct ifreq));
    strncpy(ethreq.ifr_name, ifname.c_str(), IFNAMSIZ);
    if (ioctl(fd, SIOCGIFFLAGS, &ethreq) == -1) {
        ETH_ERR("SIOCGIFFLAGS %s", strerror(errno));
        return false;
    }

    ethreq.ifr_flags |= IFF_PROMISC;
    if (ioctl(fd, SIOCSIFFLAGS, &ethreq) == -1) {
        ETH_ERR("SIOCSIFFLAGS %s", strerror(errno));
        return false;
    }
    return true;
}
}  // namespace tiny_utils
