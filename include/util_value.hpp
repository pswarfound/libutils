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
    enum ValueType {
      nullValue = 0, ///< 'null' value
      intValue,      ///< signed integer value
      uintValue,     ///< unsigned integer value
      realValue,     ///< double value
      stringValue,   ///< UTF-8 string value
      booleanValue,  ///< bool value
    };
    Value();
    ~Value();
    Value(ValueType e);
    Value(int value);
    Value(unsigned int value);
    Value(int64_t value);
    Value(uint64_t value);
    Value(double value);
    Value(const char *value);
    Value(const string &value);
    Value(bool value);
    int &val_int();
    int64_t val_int64();
    double &val_double();
    string &val_string();
    Value& operator=(Value other);
    friend std::ostream & operator<< (std::ostream &out, const tiny_utils::Value &obj);

 private:
    friend class ValueImpl;
    friend class ostream;
    ValueImpl *m_impl;
};
}  // namespace tiny_utils
