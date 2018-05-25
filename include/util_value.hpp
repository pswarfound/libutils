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
  public:
     enum ValueType {
        nullValue = 0, ///< 'null' value
        intValue,      ///< signed integer value
        uintValue,     ///< unsigned integer value
        int64Value,
        uint64Value,
        floatValue,
        realValue,     ///< double value
        stringValue,   ///< UTF-8 string value
        booleanValue,  ///< bool value
    };
    
    Value();
    Value(ValueType e);
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

    void reset(ValueType e);
    int &as_int();
    unsigned int &as_uint();
    int64_t &as_int64();
    uint64_t &as_uint64();
    float &as_float();
    double &as_real();
    string &as_string();
    bool &as_bool();
    Value& operator=(const Value &other);
    friend std::ostream & operator<< (std::ostream &out, const tiny_utils::Value &obj);

 private:
    friend class ValueImpl;
    ValueImpl *m_impl;
};
}  // namespace tiny_utils
