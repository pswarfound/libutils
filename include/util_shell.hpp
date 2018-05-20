#pragma once

namespace tiny_utils {
typedef int (*shell_callback)(int argc, const char **argv);
extern void start_shell(const char *promt, shell_callback fn);
}
