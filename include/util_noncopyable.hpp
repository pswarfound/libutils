#pragma once

namespace tiny_utils {
class noncopyable
{
 protected:
    noncopyable() {}
    ~noncopyable() {}

 private:
    noncopyable(const noncopyable &);
    const noncopyable& operator=(const noncopyable&);
};
}  // namespace tiny_utils

