#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
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
#include <string>
#include <stdio.h>
#include "util_eth.hpp"
#include "util_sock.hpp"
#include "util_debug.hpp"

#define SOCK_DBG(...) DBG("Sock", __VA_ARGS__)
#define SOCK_INF(...) INF("Sock", __VA_ARGS__)
#define SOCK_ERR(...) ERR("Sock", __VA_ARGS__)
#define SOCK_WRN(...) WRN("Sock", __VA_ARGS__)
#define SOCK_FTL(...) FTL("Sock", __VA_ARGS__)
namespace tiny_utils {
BaseSock::BaseSock(int domain, int type, int protocol)
    : m_type(type), m_domain(domain), m_protocol(protocol),
        m_sockFd(-1), m_devidx(-1)
{
}

BaseSock::~BaseSock()
{
    close();
}

bool BaseSock::open()
{
    m_sockFd = socket(m_domain, m_type, htons(m_protocol));
    if (m_sockFd < 0) {
        SOCK_ERR("create sock failed. %s\n", strerror(errno));
        return false;
    }

    return true;
}

bool BaseSock::close()
{
    if (m_sockFd > 0) {
        ::close(m_sockFd);
        m_sockFd = -1;
    }

    return true;
}

bool BaseSock::setsockopt(int level, int optname, const void *optval, socklen_t optlen)
{
    if (m_sockFd < 0) {
        return false;
    }

    if (::setsockopt(m_sockFd, level, optname, optval, optlen) < 0) {
        SOCK_ERR("set sock opt. %s\n", strerror(errno));
        return false;
    }
    return true;
}

bool BaseSock::set_reusable(bool bReuseable)
{
    int so_reuse = 1;
    if (!setsockopt(SOL_SOCKET, SO_REUSEADDR, &so_reuse, sizeof(so_reuse))) {
        return false;
    }
    #if 0
    if (!setsockopt(SOL_SOCKET, SO_REUSEPORT, &so_reuse, sizeof(so_reuse))) {
        return false;
    }
    #endif
    return true;
}

bool BaseSock::set_pkt_info(bool bEnable)
{
    int on = bEnable?1:0;

    if (!setsockopt(IPPROTO_IP, IP_PKTINFO, &on, sizeof(on))) {
        return false;
    }

    return true;
}

bool BaseSock::bind_to_addr()
{
    if (m_sockFd < 0) {
        return false;
    }

    if (::bind(m_sockFd, (struct sockaddr*)&m_addrlocal, sizeof(m_addrlocal)) < 0) {
        SOCK_ERR("bind sock opt. %s\n", strerror(errno));
        return false;
    }
    return true;
}

bool BaseSock::bind_to_device(const string &dev_name)
{
    struct ifreq req;
    memset(&req, 0, sizeof(req));
    snprintf(req.ifr_name, sizeof(req.ifr_name), "%s", dev_name.c_str());
    if (!setsockopt(SOL_SOCKET, SO_BINDTODEVICE, \
      reinterpret_cast<char *>(&req), sizeof(req))) {
           SOCK_ERR("SO_BINDTODEVICE failed");
           return false;
    }

    get_netdev_index(m_sockFd, dev_name, m_devidx);
    m_strDevName = dev_name;
    return true;
}

int BaseSock::poll(size_t second)
{
    fd_set rset;
    struct timeval tv = {0, 0};

    FD_ZERO(&rset);
    FD_SET(m_sockFd, &rset);

    tv.tv_sec = second;

    int ret = select(m_sockFd + 1, &rset, NULL, NULL, &tv);
    if (ret > 0) {
        if (FD_ISSET(m_sockFd, &rset)) {
            return 1;
        } else {
            return -1;
        }
    }

    return ret;
}

bool BaseSock::set_addr_to(const char *ip, unsigned short port)
{
    m_addrto.sin_family = AF_INET;
    m_addrto.sin_port = htons(port);
    m_addrto.sin_addr.s_addr = inet_addr(ip);

    return true;
}

bool BaseSock::set_addr_local(const char *ip, unsigned short port)
{
    m_addrlocal.sin_family = AF_INET;
    m_addrlocal.sin_addr.s_addr = inet_addr(ip);
    m_addrlocal.sin_port = htons(port);

    return true;
}

bool BaseSock::get_netdev_speed(const string &dev_name, int &speed)
{
    return ::get_netdev_speed(m_sockFd, dev_name, speed);
}

bool BaseSock::get_netdev_hwaddr(const string &dev_name, unsigned char *hwaddr)
{
    return ::get_netdev_hwaddr(m_sockFd, dev_name, hwaddr);
}

SockUDP::SockUDP()
    : BaseSock(AF_INET, SOCK_DGRAM, 0)
{
}

SockUDP::~SockUDP()
{
}

bool SockUDP::set_broadcast()
{
    int so_broadcast = 1;
    if (!setsockopt(SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast))) {
        return false;
    }
    return true;
}

bool SockUDP::sendn(const void *pData, size_t count)
{
    int ret;

    ret = sendto(m_sockFd, pData, count, 0,  (struct sockaddr*)&m_addrto,
                                        sizeof(m_addrto));
    if (ret != static_cast<int>(count)) {
        SOCK_ERR("%d %s\n", m_sockFd, strerror(errno));
        return false;
    }

    return true;
}

int SockUDP::recv_ex(void *pDst, size_t count, string &srcip)
{
    int ret;
    struct msghdr hdr;
    struct iovec iov;
    char control[CMSG_SPACE(64)];

    hdr.msg_name = &m_addrlocal;
    hdr.msg_control = control;
    hdr.msg_controllen = sizeof(control);
    iov.iov_base = reinterpret_cast<void *>(pDst);
    iov.iov_len = count;
    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_flags = 0;

    struct cmsghdr *cmsgtmp;

    ret = recvmsg(m_sockFd, &hdr, 0);
    if (ret > 0) {
       for (cmsgtmp = CMSG_FIRSTHDR(&hdr); cmsgtmp != NULL;
                                    cmsgtmp = CMSG_NXTHDR(&hdr, cmsgtmp) ) {
           if (cmsgtmp->cmsg_level == IPPROTO_IP
                && cmsgtmp->cmsg_type == IP_PKTINFO ) {
                struct in_pktinfo *pktinfo;
                pktinfo = (struct in_pktinfo*)CMSG_DATA(cmsgtmp);
                char addr1[32] = {0}, addr2[32] = {0};
                inet_ntop(AF_INET, &pktinfo->ipi_spec_dst, addr1, sizeof(addr1));
                inet_ntop(AF_INET, &pktinfo->ipi_addr, addr2, sizeof(addr2));
                srcip = addr1;
                break;
            }
       }
    } else if (ret < 0) {
        SOCK_ERR("recv %d %s\n", m_sockFd, strerror(errno));
    }

    return ret;
}

int SockUDP::recv(void *pDst, size_t count)
{
    int ret;
    socklen_t len = sizeof(m_addrlocal);
    ret = recvfrom(m_sockFd, pDst, count, 0, (struct sockaddr*)&m_addrlocal, &len);
    return ret;
}

SockRaw::SockRaw()
    : BaseSock(PF_PACKET, SOCK_RAW, 0)
{
}

SockRaw::SockRaw(int protocol)
    : BaseSock(PF_PACKET, SOCK_RAW, protocol)
{
}

SockRaw::~SockRaw()
{
}

bool SockRaw::sendn(const void *vp_src, size_t count)
{
    int ret;
    struct sockaddr_ll sa;

    memset(&sa, 0, sizeof(sa));
    sa.sll_family = PF_PACKET;
    sa.sll_ifindex = m_devidx;
    sa.sll_protocol = htons(ETH_P_ALL);
    ret = sendto(m_sockFd, vp_src, count, 0, (struct sockaddr *)&sa, sizeof(sa));

    if (ret != static_cast<int>(count)) {
        SOCK_ERR("%d %s\n", m_sockFd, strerror(errno));
        return false;
    }

    return true;
}

int SockRaw::recv(void *vp_dst, size_t count)
{
    socklen_t len;
    struct sockaddr_ll sa;

    len = sizeof(struct sockaddr_ll);
    memset(&sa, 0, len);

    int n = recvfrom(m_sockFd, vp_dst, count, 0, (struct sockaddr *)&sa, &len);
    return n;
}

bool SockRaw::set_eth_filter(unsigned short packet_type)
{
    struct ifreq ethreq;
    struct sock_fprog Filter;

    // tcpdump -dd ether proto 0x8033
    struct sock_filter BPF_code[]= {
        { 0x28, 0, 0, 0x0000000c },
        { 0x15, 0, 1, 0x00008033 },
        { 0x6, 0, 0, 0x00000060 },
        { 0x6, 0, 0, 0x00000000 }
    };
    BPF_code[1].k = packet_type;
    // init filter settings
    Filter.len = 4;
    Filter.filter = BPF_code;

    /* Attach the filter to the socket */
    if (!setsockopt(SOL_SOCKET, SO_ATTACH_FILTER, &Filter, sizeof(Filter))) {
        SOCK_ERR("SO_ATTACH_FILTER %s", strerror(errno));
        return false;
    }

    return true;
}
}
