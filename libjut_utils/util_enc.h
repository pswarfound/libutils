#ifndef __CHKSUM_H
#define __CHKSUM_H

static inline unsigned char chksum(const unsigned char *data, size_t count)
{
    unsigned char chk = 0;

    while(count) {
       chk += data[--count];
    }
    return chk;
}

#endif

