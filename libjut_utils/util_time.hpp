#pragma once

class WDTPrivate;

class WatchDog
{
 public:
    explicit WatchDog(unsigned int ms = 0);
    ~WatchDog();
 public:
    void feed();
    bool is_timeout();
 private:
    WDTPrivate  *m_private;
};

