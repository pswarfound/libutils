#pragma once
#include <string>
#include <list>

using std::string;
using std::list;

#define MAC_FMT     "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_LIST(x) x[0], x[1], x[2], x[3], x[4], x[5]

typedef struct
{
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    unsigned short proto;
}eth_header_t;

bool is_loopback(const string &ip);
bool get_netdev_index(int fd, const string &ifname, int &idx);
bool get_netdev_ipaddr(int fd, const string &ifname, string &ip);
bool get_netdev_netmask(int fd, const string &ifname, string &netmask);
bool get_netdev_hwaddr(int fd, const string &ifname, unsigned char hwaddr[6]);
bool get_netdev_namelist(list<string> &lst);
bool get_netdev_speed(int fd, const string &netdev, int &speed);
bool get_netdev_driver(const string &ifname, string &drv_name);
bool set_netdev_promiscuos(int fd, const string &ifname);

