#pragma once
#include <stdint.h>
#include <string>
#include <iostream>

using std::string;

namespace tiny_utils {
class ValueImpl;

class Value
{
 public:
    Value();
    ~Value();
    
    Value(int value);
    Value(unsigned int value);
    Value(int64_t value);
    Value(uint64_t value);
    Value(float value);
    Value(double value);
    Value(const char *value);
    Value(const string &value);
    Value(bool value);
    int &val_int();
    unsigned int &val_uint();
    int64_t &val_int64();
    uint64_t &val_uint64();
    float &val_float();
    double &val_double();
    string &val_string();
    bool &val_bool();
    Value& operator=(const Value &other);
    friend std::ostream & operator<< (std::ostream &out, const tiny_utils::Value &obj);

 private:
    friend class ValueImpl;
//    friend class ostream;
    ValueImpl *m_impl;
};
}  // namespace tiny_utils
