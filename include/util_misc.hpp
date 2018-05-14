#pragma once
#include <string>
#include <list>

using std::string;
using std::list;

namespace tiny_utils {

bool util_create_pidfile(const char *path);

void str_split(const string &s, list<string> &lst);
} // namespace tiny_utils
