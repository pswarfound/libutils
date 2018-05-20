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

    ValueType type_ : 8;
    unsigned int allocated_ : 1; // Notes: if declared as bool, bitfield is useless.

    ValueImpl()
        : type_(nullValue), allocated_(0)
    {
        memset(&value_, 0, sizeof(value_));
    }

    ~ValueImpl() {
        releasePayload();
        value_.uint64_ = 0;
    }
    
    void initBasic(ValueType vtype, bool allocated = false) {
        type_ = vtype;
        allocated_ = allocated;
    }
    
    void releasePayload() {
        switch (type_) {
            case stringValue:
                if (allocated_ && value_.string_) {
                    delete value_.string_;
                    value_.string_ = NULL;
                }
            break;
        }
        type_ = nullValue;
        allocated_ = 0;
        value_.uint64_ = 0;
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

int &Value::val_int()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::intValue;
    return m_impl->value_.int_;
}

unsigned int &Value::val_uint()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::uintValue;
    return m_impl->value_.uint_;
}

int64_t &Value::val_int64()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::int64Value;
    return m_impl->value_.int64_;
}

uint64_t &Value::val_uint64()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::uint64Value;
    return m_impl->value_.uint64_;
}

float &Value::val_float()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::floatValue;
    return m_impl->value_.float_;
}

double &Value::val_double()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::realValue;
    return m_impl->value_.real_;
}

string &Value::val_string()
{
    m_impl->releasePayload();
    m_impl->allocated_ = 1;
    m_impl->value_.string_ = new string();    
    m_impl->type_ = ValueImpl::stringValue;
    return *(m_impl->value_.string_);
}

bool &Value::val_bool()
{
    m_impl->releasePayload();
    m_impl->type_ = ValueImpl::booleanValue;
    return m_impl->value_.bool_;
}

Value::Value(int value) 
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::intValue);
  m_impl->value_.int_ = value;
}

Value::Value(unsigned int value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::uintValue);
  m_impl->value_.uint_ = value;
}

Value::Value(int64_t value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::int64Value);
  m_impl->value_.int64_ = value;
}

Value::Value(uint64_t value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::uint64Value);
  m_impl->value_.uint64_ = value;
}

Value::Value(float value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::floatValue);
  m_impl->value_.float_ = value;
}

Value::Value(double value)
    : m_impl(new ValueImpl)
{
  m_impl->initBasic(ValueImpl::realValue);
  m_impl->value_.real_ = value;
}

Value::Value(const char *value)
    : m_impl(new ValueImpl)
{  
    m_impl->initBasic(ValueImpl::stringValue, true);
    m_impl->value_.string_ = new string(value);
}

Value::Value(const string &value)
    : m_impl(new ValueImpl)
{
    m_impl->initBasic(ValueImpl::stringValue, true);
    m_impl->value_.string_ = new string(value);
}

Value::Value(bool value)
    : m_impl(new ValueImpl)
{  
    m_impl->initBasic(ValueImpl::booleanValue);
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
            case ValueImpl::intValue:
                out << obj.m_impl->value_.int_;
                break;
        case ValueImpl::uintValue:
                out << obj.m_impl->value_.uint_;
                break;
        case ValueImpl::int64Value:
                out << obj.m_impl->value_.int64_;
                break;
        case ValueImpl::uint64Value:
                out << obj.m_impl->value_.uint64_;
                break;
        case ValueImpl::floatValue:
                out << obj.m_impl->value_.float_;
                break;
        case ValueImpl::realValue:
                out << obj.m_impl->value_.real_;
                break;
        case ValueImpl::stringValue:
                out << *(obj.m_impl->value_.string_);
                break;
        case ValueImpl::booleanValue:
                out << obj.m_impl->value_.bool_?"true":"false";
                break;
        default:
            break;
        }
    }
    
    return out;
}
}

