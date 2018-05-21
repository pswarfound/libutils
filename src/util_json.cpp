#if defined(ENABLE_JSON)
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
#include "util_misc.hpp"

using namespace rapidjson;
using std::string;
using std::list;
using std::ofstream;

#define CATCH_ERROR do{m_private->m_errs="internal error";}while(0)
namespace tiny_utils {
class JsonImpl {
 public:
    JsonImpl() 
        :   m_errno(kParseErrorNone),
            m_allocator(m_doc.GetAllocator()),
            m_val(&m_doc) 
    {
    }
    ~JsonImpl()
    {

    }

    bool get_obj_value(const char *key, const Value **val);
    bool get_obj_value(const char *key, Value **val);
    bool get_array_value(size_t idx, const Value **val);
    bool get_array_value(size_t idx, Value **val);
    int m_errno;
    string m_errs;
    list<string> m_track;
    Document m_doc;
    Document::AllocatorType& m_allocator;
    Value   *m_val;

    Document::MemberIterator  m_memberBegin;
    Document::MemberIterator  m_memberEnd;
    Document::ValueIterator   m_valueBegin;
    Document::ValueIterator   m_valueEnd;
};

bool JsonImpl::get_obj_value(const char *key, const Value **val)
{
     if (!key || !m_val) {
        m_errno = JsonHelper::eInvalidParam;
        return false;
    }
    
    m_errno = JsonHelper::eNoErr;
    try {
        if (m_val->IsObject()) {
            Value::ConstMemberIterator itr = m_val->FindMember(key); 
            if (itr != m_val->MemberEnd()) {
                *val = &(itr->value);
                return true;
            } else {
                m_errno = JsonHelper::eNoEntry;
            }
        } else {
            m_errno = JsonHelper::eNotObject;
        }
    } catch(...) {
        m_errno = JsonHelper::eInternalError;
    }
    return false;
}

bool JsonImpl::get_obj_value(const char *key,  Value **val)
{
     if (!key || !m_val) {
        m_errno = JsonHelper::eInvalidParam;
        return false;
    }
    
    m_errno = JsonHelper::eNoErr;
    try {
        if (m_val->IsObject()) {
            Value::MemberIterator itr = m_val->FindMember(key); 
            if (itr != m_val->MemberEnd()) {
                *val = &(itr->value);
                return true;
            } else {
                m_errno = JsonHelper::eNoEntry;
            }
        } else {
            m_errno = JsonHelper::eNotObject;
        }
    } catch(...) {
        m_errno = JsonHelper::eInternalError;
    }
    return false;
}

bool JsonImpl::get_array_value(size_t idx, const Value **val)
{   
    return get_array_value(idx, (Value**)val);
}

bool JsonImpl::get_array_value(size_t idx, Value **val)
{
    if (!m_val->IsArray()) {
        m_errno = JsonHelper::eTypeMissmatch;
        return false;
    }
    
    m_errno = JsonHelper::eNoErr;
    if (idx >= m_val->Size()) {
        m_errno = JsonHelper::eOutOfRange;
        return false;
    }
    *val = &(*m_val)[idx];
    
    return true;
}

JsonHelper::JsonHelper()
    : m_private(new JsonImpl)
{
    m_private->m_doc.SetObject();
}

JsonHelper::~JsonHelper()
{
    if (m_private) {
        delete m_private;
    }
}

bool JsonHelper::load_from_file(const char *path) {
    m_private->m_errno = eNoErr;

    if (!path) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    try {
        std::ifstream ifs(path);
        IStreamWrapper isw(ifs);
        ParseResult ok = m_private->m_doc.ParseStream(isw);
        if (!ok) {
            m_private->m_errno = ok.Code();
            return false;
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }

    return true;
}

bool JsonHelper::load_from_data(const char *data) {
    if (!data) {
        m_private->m_errno = JsonHelper::eInvalidParam;
        return false;
    }

    try {
        ParseResult ok = m_private->m_doc.Parse(data);
        if (!ok) {
            m_private->m_errno = ok.Code();
            return false;
        }
    } catch(...) {
        m_private->m_errno = JsonHelper::eInternalError;
        return false;
    }
    return true;
}

int JsonHelper::get_errno(void)
{
    return m_private->m_errno;
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
        m_private->m_errno = JsonHelper::eInternalError;
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
        m_private->m_errno = JsonHelper::eInternalError;
       return false;
    }
    return true;
}

bool JsonHelper::clear() {
    try {
        m_private->m_doc.SetObject();
        m_private->m_val = &m_private->m_doc;
        m_private->m_track.clear();
    } catch(...) {
        return false;
    }
    return true;
}

bool JsonHelper::get(const char *key, string *out) {
    if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }

    if (!val->IsString()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }
    
    *out = val->GetString();
    
    return true;
}

bool JsonHelper::get(const char *key, int *out) {
    if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsInt()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }
    *out = val->GetInt();
    
    return true;
}

bool JsonHelper::get(const char *key, unsigned int *out) {
  if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsUint()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }  
    *out = val->GetUint();
    
    return true;
}

bool JsonHelper::get(const char *key, float *out) {
  if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsFloat()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }   
    *out = val->GetFloat();
    
    return true;
}

bool JsonHelper::get(const char *key, double *out) {
  if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsDouble()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }     
    *out = val->GetDouble();
    
    return true;
}

bool JsonHelper::get(const char *key, int64_t *out) {
  if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsInt64()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }      
    *out = val->GetInt64();
    
    return true;
}

bool JsonHelper::get(const char *key, uint64_t *out) {
    if (!key || !out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    const Value *val = NULL;
    if (!m_private->get_obj_value(key, &val) || val == NULL) {
        return false;
    }
    if (!val->IsUint64()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }     
    *out = val->GetUint64();
    
    return true;
}

bool JsonHelper::set(const char *key, const char *in, bool bCreat) {
    if (!key || !in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsString()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetString(in, strlen(in), m_private->m_allocator);
        } else {
            // no entry, create one
            Value val_key(kStringType), val_value(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            val_value.SetString(in, strlen(in), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, val_value, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
       return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, const string &in, bool bCreat) {
    return set(key, in.c_str(), bCreat);
}

bool JsonHelper::set(const char *key, int in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsInt()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetInt(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, unsigned int in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsUint()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetUint(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, float in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsFloat()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetFloat(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, const double &in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsDouble()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetDouble(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, const int64_t &in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsInt64()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetInt64(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(const char *key, const uint64_t &in, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsUint64()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            
            val->SetUint64(in);
        } else {
            // no entry, create one
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::locate_obj(const char *key, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsObject()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            m_private->m_val = val;
            m_private->m_track.push_back(key);
        } else {
            Value obj(kObjectType);
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, obj, m_private->m_allocator);
            return locate_obj(key);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::locate_array(const char *key, bool bCreat) {
    if (!key) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    Value *val = NULL;
    if (((!m_private->get_obj_value(key, &val) || val == NULL)
        && m_private->m_errno != eNoEntry)
        || (m_private->m_errno == eNoEntry && !bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsArray()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            m_private->m_val = val;
            m_private->m_track.push_back(key);
        } else {
            Value obj(kArrayType);
            Value val_key(kStringType);
            val_key.SetString(key, strlen(key), m_private->m_allocator);
            m_private->m_val->AddMember(val_key, obj, m_private->m_allocator);
            return locate_array(key);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;

}

bool JsonHelper::out(int step) {
    if (step <=0) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    // at root
    if (m_private->m_track.empty()) {
        m_private->m_errno = eOutOfRange;
        return false;
    }
    try {
        while (step-- && !m_private->m_track.empty()) {
            m_private->m_track.pop_back();
        }
        m_private->m_val = &m_private->m_doc;
        // at root
        if (m_private->m_track.empty()) {
            return true;
        }
        list<string> path;
        path.assign(m_private->m_track.begin(), m_private->m_track.end());
        m_private->m_track.clear();

        list<string>::iterator iter = path.begin();
        while (iter != path.end()) {
            if ((*iter)[0] == '\\') {
                size_t idx = atoi(iter->c_str() + 1);
                if (!locate_obj(idx) && !locate_array(idx)) {
                    m_private->m_track.clear();
                    m_private->m_val = &m_private->m_doc;
                    m_private->m_errno = eNoEntry;
                    return false;
                }                
            } else {
                if (!locate_obj(iter->c_str()) && !locate_array(iter->c_str())) {
                    m_private->m_track.clear();
                    m_private->m_val = &m_private->m_doc;
                    m_private->m_errno = eNoEntry;
                    return false;
                }
            }
            iter++;
        }
    } catch(...) {
        m_private->m_track.clear();
        m_private->m_val = &m_private->m_doc;
        m_private->m_errno = eInternalError;
        return false;
    }
    return true;
}

static bool parse_path(const char *path, list<string> &lst)
{
    str_split(path, '/', lst);
    return true;
}

bool JsonHelper::direct(const char *path)
{
    list<string> lst;

    parse_path(path, lst);
    
    m_private->m_errno = eNoErr;
    m_private->m_track.clear();
    m_private->m_val = &m_private->m_doc;
    if (lst.empty()) {
        m_private->m_errno = eInvalidParam;
        return false;
    }

    list<string>::const_iterator iter = lst.begin();
    while (iter != lst.end()) {
        if ((*iter)[0] == '\\') {
            size_t idx = atoi(iter->c_str() + 1);
            if (!locate_obj(idx) && !locate_array(idx)) {
                return false;
            }
        } else {
            if (!locate_obj(iter->c_str())
                && !locate_array(iter->c_str())) {
                m_private->m_track.clear();
                m_private->m_val = &m_private->m_doc;
                m_private->m_errno = eNoEntry;
                return false;
            }
        }
        iter++;
    }
    return true;
}

bool JsonHelper::get(size_t idx, string *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsString()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetString();
    return true;
}

bool JsonHelper::get(size_t idx, int *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsInt()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetInt();
    return true;
}

bool JsonHelper::get(size_t idx, int64_t *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsInt64()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetInt64();
    return true;
}

bool JsonHelper::get(size_t idx, unsigned int *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsUint()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetUint();
    return true;
}

bool JsonHelper::get(size_t idx, uint64_t *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsUint64()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetUint64();
    return true;
}

bool JsonHelper::get(size_t idx, float *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsFloat()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetFloat();
    return true;
}

bool JsonHelper::get(size_t idx, double *out)
{
    if (!out) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    const Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)) {
        return false;
    }
    
    if (!val->IsDouble()) {
        m_private->m_errno = eTypeMissmatch;
        return false;
    }

    *out = val->GetDouble();
    return true;
}


bool JsonHelper::set(size_t idx, const char *in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsString()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetString(in, strlen(in), m_private->m_allocator);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsString()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            Value val_value(kStringType);
            val_value.SetString(in, strlen(in), m_private->m_allocator);
            m_private->m_val->PushBack(val_value, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, const string &in, bool bCreat)
{
    return set(idx, in.c_str(), bCreat);
}

bool JsonHelper::set(size_t idx, float in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsFloat()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetFloat(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsFloat()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, double &in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsDouble()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetDouble(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsDouble()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, int in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsInt()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetInt(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsInt()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, const int64_t &in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsInt64()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetInt64(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsInt64()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, unsigned int in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsUint()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetUint(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsUint()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::set(size_t idx, const uint64_t &in, bool bCreat)
{
    if (!in) {
        m_private->m_errno = eInvalidParam;
        return false;
    }
    
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }
    try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsUint64()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            val->SetUint64(in);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsUint64()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            m_private->m_val->PushBack(in, m_private->m_allocator);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}

bool JsonHelper::locate_obj(size_t idx, bool bCreat)
{
    Value *val = NULL;
    if (!m_private->get_array_value(idx, &val)
        && !(m_private->m_errno == eOutOfRange 
                && idx == m_private->m_val->Size()
                && bCreat)) {
        return false;
    }

    try {
       if (m_private->m_errno == eNoErr) {
           if (!val->IsObject()) {
               m_private->m_errno = eTypeMissmatch;
               return false;
           }
           char buf[32];
           snprintf(buf, sizeof(buf), "\%d", idx);
           m_private->m_val = val;
           m_private->m_track.push_back(buf);
       } else if (idx > 0 && !(*m_private->m_val)[0].IsObject()) {
           m_private->m_errno = eTypeMissmatch;
           return false;
       } else {
           Value obj(kObjectType);
           m_private->m_val->PushBack(obj, m_private->m_allocator);
           return locate_obj(idx);
       }
   } catch(...) {
       m_private->m_errno = eInternalError;
       return false;
   }
   
   return true;
}

bool JsonHelper::locate_array(size_t idx, bool bCreat)
{
     Value *val = NULL;
     if (!m_private->get_array_value(idx, &val)
         && !(m_private->m_errno == eOutOfRange 
                 && idx == m_private->m_val->Size()
                 && bCreat)) {
        return false;
     }
    
     try {
        if (m_private->m_errno == eNoErr) {
            if (!val->IsArray()) {
                m_private->m_errno = eTypeMissmatch;
                return false;
            }
            char buf[32];
            snprintf(buf, sizeof(buf), "\%d", idx);
            m_private->m_val = val;
            m_private->m_track.push_back(buf);
        } else if (idx > 0 && !(*m_private->m_val)[0].IsArray()) {
            m_private->m_errno = eTypeMissmatch;
            return false;
        } else {
            Value obj(kArrayType);
            m_private->m_val->PushBack(obj, m_private->m_allocator);
            return locate_array(idx);
        }
    } catch(...) {
        m_private->m_errno = eInternalError;
        return false;
    }
    
    return true;
}


} // namespace tiny_utils
#endif  // #if defined(ENABLE_JSON)
