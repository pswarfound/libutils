#if defined(ENABLE_JSON)
#pragma once
#include <stdint.h>
#include <string>

using std::string;

namespace tiny_utils {
class JsonImpl;

class JsonHelper
{
 public:
    typedef enum {
        eNoErr = 0,
        eInternalError = 100,
        eInvalidParam,
        eNotObject,
        eNotArray,
        eTypeMissmatch,
        eNoEntry,
    }js_errno_e;
    
    JsonHelper();
    ~JsonHelper();
    
    bool load_from_file(const char *path);
    bool load_from_data(const char *data);
    bool save_to_file(const char *path, bool bPretty = true);
    bool get_doc(string &, bool bPretty = true);
    int get_errno(void);
    const string &get_error() const;
    bool current(string &key);
    /// for locate then get/set operations
    bool get(const char *key, string *sval); 
    bool get(const char *key, int *ival);
    bool get(const char *key, int64_t *ival);
    bool get(const char *key, unsigned int *ival);
    bool get(const char *key, uint64_t *ival);
    bool get(const char *key, float *fval);
    bool get(const char *key, double *fval);

    bool set(const char *key, const char *val, bool bCreat = false);
    bool set(const char *key, const string &val, bool bCreat = false);
    bool set(const char *key, float fval, bool bCreat = false);
    bool set(const char *key, double fval, bool bCreat = false);
    bool set(const char *key, int ival, bool bCreat = false);
    bool set(const char *key, const int64_t &fval, bool bCreat = false);
    bool set(const char *key, unsigned int ival, bool bCreat = false);
    bool set(const char *key, const uint64_t &fval, bool bCreat = false);

    bool locate_obj(const char *key, bool bCreat = false);
    bool out_obj();
    
    #if 0
    bool locate_array(const char *key, bool bCreat = false);
    #endif
    #if 0
    bool get(size_t idx, string &sval);    
    bool get(size_t idx, int &ival);    
    bool get(size_t idx, float &fval);    
    bool set(size_t idx, const char *sval);
    bool set(size_t idx, int ival);
    bool set(size_t idx, float fval);
    
    bool add_string(const char *sval);
    bool add_int(int ival);
    bool add_float(float fval);
    bool add_obj(int &idx);
    bool add_array(int &idx);
    bool locate_obj(size_t idx);
    bool locate_array(size_t idxe);
    int size(); 
     bool clear();  
   #endif
 private:
    JsonImpl *m_private;
};

#define JS_DBG(...) DBG("JS", __VA_ARGS__)
#define JS_INF(...) INF("JS", __VA_ARGS__)
#define JS_ERR(...) ERR("JS", __VA_ARGS__)
#define JS_WRN(...) WRN("JS", __VA_ARGS__)
#define JS_FTL(...) FTL("JS", __VA_ARGS__)
#define JS_VAR(lv, ...) DBG_VAR(lv, "JS", __VA_ARGS__)

}
#endif  // #if defined(ENABLE_JSON)
