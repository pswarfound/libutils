#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include "util_value.hpp"

using std::string;
namespace tiny_utils {
class ValueImpl
{
 public:
    typedef int Int;
    typedef unsigned int UInt;
    typedef int64_t Int64;
    typedef uint64_t UInt64;
    typedef Int64 LargestInt;
    typedef UInt64 LargestUInt;

    union ValueHolder {
        Int int_;
        UInt uint_;
        double real_;
        bool bool_;
        string *string_;
    } value_;

    Value::ValueType type_ : 8;
    unsigned int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.

    ValueImpl() {
    }

    ~ValueImpl() {
        releasePayload();
        value_.uint_ = 0;
    }
    
    void initBasic(Value::ValueType vtype, bool allocated = false) {
        type_ = vtype;
        allocated_ = allocated;
    }
    void releasePayload() {
        switch (type_) {
        case Value::nullValue:
        case Value::intValue:
        case Value::uintValue:
        case Value::realValue:
        case Value::booleanValue:
            break;
        case Value::stringValue:
            if (allocated_) {
                delete (value_.string_);
                value_.string_ = NULL;
            }
            break;
        }
    }
    
    void swapPayload(Value& other) {
        Value::ValueType temp = type_;
        type_ = other.m_impl->type_;
        other.m_impl->type_ = temp;
        std::swap(value_, other.m_impl->value_);
        int temp2 = allocated_;
        allocated_ = other.m_impl->allocated_;
        other.m_impl->allocated_ = temp2 & 0x1;
    }
    
    void swap(Value& other) {
        swapPayload(other);
    }
};



Value::Value()
    : m_impl(new ValueImpl)
{
}

Value::~Value()
{
    if (m_impl) {
        delete m_impl;
    }
}

Value::Value(ValueType e)
{
    if (m_impl->allocated_) {
        delete m_impl->value_.string_;
        m_impl->value_.string_ = NULL;
    }
    
    m_impl->type_ = e;
    if (e == stringValue) {
        m_impl->value_.string_ = new string;
    }
}

int &Value::val_int()
{
    return m_impl->value_.int_;
}

int64_t Value::val_int64()
{
    return m_impl->value_.int_;
}

double &Value::val_double()
{
    return m_impl->value_.real_;
}

string &Value::val_string()
{
    if (!m_impl->value_.string_) {
        m_impl->allocated_ = true;
        m_impl->value_.string_ = new string();
    }
    
    return *(m_impl->value_.string_);
}

Value::Value(int value) 
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(intValue);
  m_impl->value_.int_ = value;
}

Value::Value(unsigned int value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(uintValue);
  m_impl->value_.uint_ = value;
}

Value::Value(int64_t value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(intValue);
  m_impl->value_.int_ = value;
}

Value::Value(uint64_t value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(uintValue);
  m_impl->value_.uint_ = value;
}

Value::Value(double value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(realValue);
  m_impl->value_.real_ = value;
}

Value::Value(const char *value)
    : m_impl(new ValueImpl)
{  
    m_impl->initBasic(stringValue, true);
    m_impl->value_.string_ = new string(value);
//    m_impl->value_.string_ = strdup(value);
}

Value::Value(const string &value)
    : m_impl(new ValueImpl)
{
    m_impl->initBasic(stringValue, true);
    m_impl->value_.string_ = new string(value);
//    m_impl->value_.string_ = strdup(value.c_str());
}

Value::Value(bool value)
    : m_impl(new ValueImpl)
{  
    m_impl->initBasic(booleanValue);
    m_impl->value_.bool_ = value;
}

Value& Value::operator=(Value other) {
  m_impl->swap(other);
  return *this;
}

std::ostream & operator<<(std::ostream &out, const tiny_utils::Value& obj)
{
    if (obj.m_impl) {
        switch (obj.m_impl->type_) {
            case Value::intValue:
                out << obj.m_impl->value_.int_;
                break;
        case Value::uintValue:
                out << obj.m_impl->value_.uint_;
                break;
        case Value::realValue:
                out << obj.m_impl->value_.real_;
                break;
        case Value::stringValue:
                out << obj.m_impl->value_.string_;
                break;
        case Value::booleanValue:
                out << obj.m_impl->value_.bool_?"true":"false";
                break;
        default:
            break;
        }
    }
    
    return out;
}
}

