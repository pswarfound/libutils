#pragma once
#include <netinet/in.h>
#include <string>
#include <list>

using std::string;
using std::list;

class BaseSock
{
 public:
    BaseSock(int domain, int type, int protocol);
    virtual ~BaseSock();
 public:
    virtual bool close();
    virtual bool open();
    virtual int poll(size_t ms);
    virtual bool sendn(const void *pData, size_t count) = 0;
    virtual int recv(void *pDst, size_t count) = 0;

    bool set_reusable(bool bReuseable);
    bool set_addr_to(const char *ip, unsigned short port);
    bool set_pkt_info(bool bEnable);
    bool set_addr_local(const char *ip, unsigned short port);
    bool bind_to_addr();
    bool bind_to_device(const string &dev_name);

    bool get_netdev_hwaddr(const string &netdev, unsigned char *hwaddr);
    bool get_netdev_speed(const string &dev_name, int &speed);
 protected:
    virtual bool setsockopt(int level, int optname, const void *optval, socklen_t optlen);
 protected:
    int m_type;
    int m_domain;
    int m_protocol;
    int m_sockFd;
    string m_strDevName;
    int m_devidx;
    struct sockaddr_in m_addrto;
    struct sockaddr_in m_addrlocal;
};

class SockUDP : public BaseSock
{
 public:
    SockUDP();
    ~SockUDP();
 public:
    bool set_broadcast();
    virtual bool sendn(const void *pData, size_t count);
    virtual int recv(void *pDst, size_t count);
    virtual int recv_ex(void *pDst, size_t count, string &srcip);
};

class SockRaw: public BaseSock
{
 public:
    SockRaw();
    explicit SockRaw(int protocol);
    ~SockRaw();
 public:
    virtual bool sendn(const void *vp_src, size_t count);
    virtual bool set_eth_filter(unsigned short packet_type);
    virtual int recv(void *vp_dst, size_t count);
};

