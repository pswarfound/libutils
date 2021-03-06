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
        eOutOfRange,
    }js_errno_e;
    
    JsonHelper();
    ~JsonHelper();
    
    bool load_from_file(const char *path);
    bool load_from_data(const char *data);
    bool save_to_file(const char *path, bool bPretty = true);
    bool get_doc(string &, bool bPretty = true);
    bool clear(void);
    int get_errno(void);
    const string &get_error() const;

    /// for locate then get/set operations
    bool get(const char *key, string *val); 
    bool get(const char *key, int *val);
    bool get(const char *key, int64_t *val);
    bool get(const char *key, unsigned int *val);
    bool get(const char *key, uint64_t *val);
    bool get(const char *key, float *val);
    bool get(const char *key, double *val);

    bool set(const char *key, const char *val, bool bCreat = false);
    bool set(const char *key, const string &val, bool bCreat = false);
    bool set(const char *key, float val, bool bCreat = false);
    bool set(const char *key, const double &val, bool bCreat = false);
    bool set(const char *key, int val, bool bCreat = false);
    bool set(const char *key, const int64_t &val, bool bCreat = false);
    bool set(const char *key, unsigned int val, bool bCreat = false);
    bool set(const char *key, const uint64_t &val, bool bCreat = false);

    bool locate_obj(const char *key, bool bCreat = false);
    bool locate_array(const char *key, bool bCreat = false);
    bool out(int step = 1);


    /// for array operation
    bool get(size_t idx, string *val); 
    bool get(size_t idx, int *val);
    bool get(size_t idx, int64_t *val);
    bool get(size_t idx, unsigned int *val);
    bool get(size_t idx, uint64_t *val);
    bool get(size_t idx, float *val);
    bool get(size_t idx, double *val);

    bool set(size_t idx, const char *val, bool bCreat = false);
    bool set(size_t idx, const string &val, bool bCreat = false);
    bool set(size_t idx, float val, bool bCreat = false);
    bool set(size_t idx, double &val, bool bCreat = false);
    bool set(size_t idx, int val, bool bCreat = false);
    bool set(size_t idx, const int64_t &val, bool bCreat = false);
    bool set(size_t idx, unsigned int val, bool bCreat = false);
    bool set(size_t idx, const uint64_t &val, bool bCreat = false);
    
    bool locate_obj(size_t idx, bool bCreat = false);
    bool locate_array(size_t idx, bool bCreat = false);

    bool direct(const char *path);

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
