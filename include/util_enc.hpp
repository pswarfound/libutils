#pragma once

namespace tiny_utils {
static inline unsigned char chksum(const unsigned char *data, size_t count)
{
    unsigned char chk = 0;

    while(count) {
       chk += data[--count];
    }
    return chk;
}

} // namespace tiny_utils