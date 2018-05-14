#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/error/en.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "util_json.hpp"

using namespace rapidjson;
using std::string;
using std::list;
using std::ofstream;

#define CATCH_ERROR do{m_private->m_errs="internal error";}while(0)
namespace tiny_utils {
class JsonPrivate {
 public:
    JsonPrivate() 
        : m_allocator(m_doc.GetAllocator()),
        m_val(&m_doc) 
    {
        m_track.push_back(m_val);
    }
    ~JsonPrivate()
    {

    }
    
    string m_errs;
    list<Value *> m_track;
    Document m_doc;
    Document::AllocatorType& m_allocator;
    Value   *m_val;

    Document::MemberIterator  m_memberBegin;
    Document::MemberIterator  m_memberEnd;
    Document::ValueIterator   m_valueBegin;
    Document::ValueIterator   m_valueEnd;
};

JsonHelper::JsonHelper()
    : m_private(new JsonPrivate)
{
}

JsonHelper::~JsonHelper()
{
    if (m_private) {
        delete m_private;
    }
}

bool JsonHelper::load_from_file(const char *path) {
    if (!path) {
        m_private->m_errs = "invalid param";
        return false;
    }
    
    try {
        std::ifstream ifs(path);
        IStreamWrapper isw(ifs);
        ParseResult ok = m_private->m_doc.ParseStream(isw);
        if (!ok) {
            m_private->m_errs = GetParseError_En(ok.Code());
            return false;
        }
    } catch(...) {
        m_private->m_errs = "internal error";
        return false;
    }

    return true;
}

bool JsonHelper::load_from_data(const char *data) {
    if (!data) {
        return false;
    }

    try {
        ParseResult ok = m_private->m_doc.Parse(data);
        if (!ok) {
            m_private->m_errs = GetParseError_En(ok.Code());
            return false;
        }
    } catch(...) {
        return false;
    }
    return true;
}

bool JsonHelper::get_doc(string &out, bool bPretty) {
    try {
        StringBuffer buffer;
        if (bPretty) {
            PrettyWriter<StringBuffer> writer(buffer);
            m_private->m_doc.Accept(writer);
        } else {
            Writer<StringBuffer> writer(buffer);
            m_private->m_doc.Accept(writer);
        }
        out = buffer.GetString();
    } catch(...) {
        return false;
    }
    return true;
}

const string &JsonHelper::get_error() const
{
    return m_private->m_errs;
};

bool JsonHelper::save_to_file(const char *path, bool bPretty) {
    try {
        string s;
        if (!get_doc(s, bPretty)) {
            return false;
        }
        ofstream outfile;
        outfile.open(path);
        if (outfile.fail()) {
            return false;
        }

        outfile << s;
        outfile.close();
    } catch(...) {
        return false;
    }
    return true;
}

bool JsonHelper::clear() {
    try {
        m_private->m_doc.SetObject();
        m_private->m_val = &m_private->m_doc;
        m_private->m_track.clear();
        m_private->m_track.push_back(m_private->m_val);
    } catch(...) {
        return false;
    }
}

bool JsonHelper::get_string(const char *key, string &out) {
    try {
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::ConstMemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()
                && itr->value.IsString()) {
                out = itr->value.GetString();
                return true;
            } else {
                m_private->m_errs = "no such key";
            }
        } else {
            m_private->m_errs = "invalid param";
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::get_int(const char *key, int &out) {
    try {
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::ConstMemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()
                && itr->value.IsInt()) {
                out = itr->value.GetInt();
                return true;
            } else {
                m_private->m_errs = "no such key";
            }
        } else {
            m_private->m_errs = "invalid param";
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::get_float(const char *key, float &fval) {
    try {
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::ConstMemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()
                && itr->value.IsFloat()) {
                fval = itr->value.GetFloat();
                return true;
            } else {
                m_private->m_errs = "no such key";
            }
        } else {
            m_private->m_errs = "invalid param";
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::set_string(const char *key, const char *val, bool bCreat) {
    try {        
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::MemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()) {
                if (itr->value.IsString()) {
                    itr->value.SetString(val, strlen(val), m_private->m_allocator);
                    return true;
                }
            } else if (bCreat) {
                Value val_key(kStringType), val_value(kStringType);
                val_key.SetString(key, strlen(key), m_private->m_allocator);
                val_value.SetString(val, strlen(val), m_private->m_allocator);
                m_private->m_val->AddMember(val_key, val_value, m_private->m_allocator);
                return true;
            }
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::set_int(const char *key, int val, bool bCreat) {
    try {        
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::MemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()) {
                if (itr->value.IsInt()) {
                    itr->value.SetInt(val);
                    return true;
                }
            } else if (bCreat) {
                Value val_key(kStringType);
                val_key.SetString(key, strlen(key), m_private->m_allocator);
                m_private->m_val->AddMember(val_key, val, m_private->m_allocator);
                return true;
            }
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::set_float(const char *key, float fval, bool bCreat) {
    try {        
        if (key && m_private->m_val && m_private->m_val->IsObject()) {
            Value::MemberIterator itr = m_private->m_val->FindMember(key); 
            if (itr != m_private->m_val->MemberEnd()) {
                if (itr->value.IsFloat()) {
                    itr->value.SetFloat(fval);
                    return true;
                }
            } else if (bCreat) {
                Value val_key(kStringType);
                val_key.SetString(key, strlen(key), m_private->m_allocator);
                m_private->m_val->AddMember(val_key, fval, m_private->m_allocator);
                return true;
            }
        }
    } catch(...) {
        CATCH_ERROR;
    }
    return false;
}

bool JsonHelper::locate_obj(const char *key, bool bCreat) {
    try {
        if (m_private->m_val && m_private->m_val->IsObject()) {
            Value::MemberIterator itr = m_private->m_val->FindMember(key);        
            if (itr != m_private->m_val->MemberEnd()) {
                if (itr->value.IsObject()) {
                    m_private->m_val = &(itr->value);
                    m_private->m_track.push_back(m_private->m_val);
                }
                return true;
            } else if (bCreat) {
                Value val(kObjectType);
                Value val_key(kStringType);
                val_key.SetString(key, strlen(key), m_private->m_allocator);
                m_private->m_val->AddMember(val_key, val, m_private->m_allocator);
                return locate_obj(key);
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::locate_array(const char *key, bool bCreat) {
    try {
        if (m_private->m_val && m_private->m_val->IsObject()) {
            Value::MemberIterator itr = m_private->m_val->FindMember(key);        
            if (itr != m_private->m_val->MemberEnd()) {
                if (itr->value.IsArray()) {
                    m_private->m_val = &(itr->value);
                    m_private->m_track.push_back(m_private->m_val);
                }
                return true;
            } else if (bCreat) {
                Value val(kArrayType);
                Value val_key(kStringType);
                val_key.SetString(key, strlen(key), m_private->m_allocator);
                m_private->m_val->AddMember(val_key, val, m_private->m_allocator);
                return locate_array(key);
            }
        }
    } catch(...) {
    }
    return false;
}

int JsonHelper::size() {
    try {
        if (m_private->m_val) {
            if (m_private->m_val->IsArray()) {
                return m_private->m_val->Size();
            } else if (m_private->m_val->IsObject()) {

                return m_private->m_val->MemberCount();
            }
        }
    } catch(...) {
    }
    return 0;
}

#if 0
bool JsonHelper::get_array_float(size_t idx, float &fval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsFloat()) {
                fval = (*m_private->m_val)[idx].GetFloat();
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::get_array_string(size_t idx, string &fval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsString()) {
                fval = (*m_private->m_val)[idx].GetString();
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::get_array_int(size_t idx, int &fval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsInt()) {
                fval = (*m_private->m_val)[idx].GetInt();
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::set_array_string(size_t idx, const char *sval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsString()) {
                (*m_private->m_val)[idx].SetString(sval, strlen(sval), m_private->m_allocator);
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::set_array_int(size_t idx, int ival) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsInt()) {
                (*m_private->m_val)[idx].SetInt(ival);
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::set_array_float(size_t idx,  float fval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsFloat()) {
                (*m_private->m_val)[idx].SetFloat(fval);
                return true;
            }
        }
    } catch(...) {
    }
    return false;

}

bool JsonHelper::locate_array_obj(size_t idx) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsObject()) {
                m_private->m_val = &(*m_private->m_val)[idx];
                m_private->m_track.push_back(m_private->m_val);
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::locate_array_array(size_t idx) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray() && idx < m_private->m_val->Size()) {
            if ((*m_private->m_val)[idx].IsArray()) {
                m_private->m_val = &(*m_private->m_val)[idx];
                m_private->m_track.push_back(m_private->m_val);
                return true;
            }
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_array_string(const char *sval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsString()) {
                return false;
            }
            Value val_value(kStringType);
            val_value.SetString(sval, strlen(sval), m_private->m_allocator);
            m_private->m_val->PushBack(val_value, m_private->m_allocator);
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_array_int(int ival) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsInt()) {
                return false;
            }
            m_private->m_val->PushBack(ival, m_private->m_allocator);
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_array_float(float fval) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsFloat()) {
                return false;
            }
            m_private->m_val->PushBack(fval, m_private->m_allocator);
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_array_array(int &idx) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsArray()) {
                return false;
            }
            Value val(kArrayType);
            m_private->m_val->PushBack(val, m_private->m_allocator);
            idx = m_private->m_val->Size() - 1;
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_array_object(int &idx) {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsObject()) {
                return false;
            }
            Value val(kObjectType);
            m_private->m_val->PushBack(val, m_private->m_allocator);
            idx = m_private->m_val->Size() - 1;
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_into_array() {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsArray()) {
                return false;
            }
            Value val(kArrayType);
            m_private->m_val->PushBack(val, m_private->m_allocator);
            m_private->m_track.push_back(m_private->m_val);
        }
    } catch(...) {
    }
    return false;
}

bool JsonHelper::add_into_obj() {
    try {
        if (m_private->m_val && m_private->m_val->IsArray()) {
            if (!m_private->m_val->Empty() && !(*m_private->m_val)[0].IsObject()) {
                return false;
            }
            Value val(kObjectType);
            m_private->m_val->PushBack(val, m_private->m_allocator);
            m_private->m_track.push_back(m_private->m_val);
        }
    } catch(...) {
    }
    return false;
}

#endif
bool JsonHelper::out() {
    if (m_private->m_track.size() > 1) {
        m_private->m_track.pop_back();
        m_private->m_val = m_private->m_track.back();
    }
    return true;
}



} // namespace tiny_utils
