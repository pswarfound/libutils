#pragma once
#include <string>

using std::string;

namespace tiny_utils {
class JsonPrivate;

class JsonHelper
{
 public:
    JsonHelper();
    ~JsonHelper();
    
    bool load_from_file(const char *path);
    bool load_from_data(const char *data);
    bool save_to_file(const char *path, bool bPretty = true);
    bool get_doc(string &, bool bPretty = true);
    const string &get_error() const;

    bool get(const char *key, string &sval);    
    bool get(const char *key, int &ival);    
    bool get(const char *key, float &fval);
    bool set(const  char *key, const char *val, bool bCreat = false);
    bool set(const  char *key, float fval, bool bCreat = false);
    bool set(const  char *key, int ival, bool bCreat = false);
    bool locate_obj(const char *key, bool bCreat = false);
    bool locate_array(const char *key, bool bCreat = false);

    bool get_string(size_t idx, string &sval);    
    bool get_int(size_t idx, int &ival);    
    bool get_float(size_t idx, float &fval);    
    bool set_string(size_t idx, const char *sval);
    bool set_int(size_t idx, int ival);
    bool set_float(size_t idx,  float fval);
    bool add_string(const char *sval);
    bool add_int(int ival);
    bool add_float(float fval);
    bool add_obj(int &idx);
    bool add_array(int &idx);
    bool locate_obj(size_t idx);
    bool locate_array(size_t idxe);

    int size();
    bool out();
    bool clear();
 private:
    JsonPrivate *m_private;
};
}
