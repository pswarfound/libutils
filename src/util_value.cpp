#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include "util_value.hpp"

using std::string;
namespace tiny_utils {
class ValueImpl
{
 public:
    union ValueHolder {
        int int_;
        unsigned int uint_;
        int64_t int64_;
        uint64_t uint64_;
        float float_;
        double real_;
        bool bool_;
        string *string_;
    } value_;

    Value::ValueType type_ : 8;
    unsigned int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.

    ValueImpl()
        : type_(Value::nullValue), allocated_(0)
    {
        memset(&value_, 0, sizeof(value_));
    }

    ValueImpl(Value::ValueType e)
        : type_(e), allocated_(0)
    {
        memset(&value_, 0, sizeof(value_));
        if (e == Value::stringValue) {
            allocated_ = 1;
            value_.string_ = new string;
        }
    }
    
    ~ValueImpl() {
        releasePayload();
        value_.uint64_ = 0;
    }
    
    void releasePayload() {
        switch (type_) {
            case Value::stringValue:
                if (allocated_ && value_.string_) {
                    delete value_.string_;
                    value_.string_ = NULL;
                }
            break;
        }
        type_ = Value::nullValue;
        allocated_ = 0;
        value_.uint64_ = 0;
    }
};



Value::Value()
    : m_impl(new ValueImpl)
{
}

Value::Value(ValueType e)
    : m_impl(new ValueImpl(e))
{
}

Value::~Value()
{
    if (m_impl) {
        delete m_impl;
    }
}

void Value::reset(ValueType e)
{
    m_impl->releasePayload();
    m_impl->type_ = e;
    if (e == stringValue) {
        m_impl->allocated_ = 1;
        m_impl->value_.string_ = new string;
    }
}

int &Value::as_int()
{
    return m_impl->value_.int_;
}

unsigned int &Value::as_uint()
{
    return m_impl->value_.uint_;
}

int64_t &Value::as_int64()
{
    return m_impl->value_.int64_;
}

uint64_t &Value::as_uint64()
{
    return m_impl->value_.uint64_;
}

float &Value::as_float()
{
    return m_impl->value_.float_;
}

double &Value::as_real()
{
    return m_impl->value_.real_;
}

string &Value::as_string()
{
    return *(m_impl->value_.string_);
}

bool &Value::as_bool()
{
    return m_impl->value_.bool_;
}

Value::Value(int value) 
    : m_impl(new ValueImpl(intValue))
{
  m_impl->value_.int_ = value;
}

Value::Value(unsigned int value)
    : m_impl(new ValueImpl(uintValue))
{
  m_impl->value_.uint_ = value;
}

Value::Value(int64_t value)
    : m_impl(new ValueImpl(int64Value))
{
  m_impl->value_.int64_ = value;
}

Value::Value(uint64_t value)
    : m_impl(new ValueImpl(uint64Value))
{
    m_impl->value_.uint64_ = value;
}

Value::Value(float value)
    : m_impl(new ValueImpl(floatValue))
{
  m_impl->value_.float_ = value;
}

Value::Value(double value)
    : m_impl(new ValueImpl(realValue))
{
  m_impl->value_.real_ = value;
}

Value::Value(const char *value)
    : m_impl(new ValueImpl(stringValue))
{  
    *m_impl->value_.string_ = value;
}

Value::Value(const string &value)
    : m_impl(new ValueImpl(stringValue))
{
    *m_impl->value_.string_ = value;
}

Value::Value(bool value)
    : m_impl(new ValueImpl(booleanValue))
{
    m_impl->value_.bool_ = value;
}

Value& Value::operator=(const Value &other) {
    if (this == &other) {
        return *this;
    }
    
    m_impl->releasePayload();
    m_impl->type_ = other.m_impl->type_;
    m_impl->allocated_ = other.m_impl->allocated_;
    if (m_impl->allocated_) {
        m_impl->value_.string_ = new string(*other.m_impl->value_.string_);
    } else {
        m_impl->value_.uint64_ = other.m_impl->value_.uint64_;
    }
   
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
            case Value::int64Value:
                    out << obj.m_impl->value_.int64_;
                    break;
            case Value::uint64Value:
                    out << obj.m_impl->value_.uint64_;
                    break;
            case Value::floatValue:
                    out << obj.m_impl->value_.float_;
                    break;
            case Value::realValue:
                    out << obj.m_impl->value_.real_;
                    break;
            case Value::stringValue:
                    out << *(obj.m_impl->value_.string_);
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

