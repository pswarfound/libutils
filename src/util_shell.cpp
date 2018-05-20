#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "util_shell.hpp"
#include "util_misc.hpp"

using std::list;
using std::string;
using std::vector;

namespace tiny_utils {

class VT100
{
 public:
    typedef enum {
        UP = 0,
        DOWN,
        RIGHT,
        LEFT
    }dir_e;
    VT100() {
        struct termios ts;
        tcgetattr(STDIN_FILENO, &ts);
        m_ts = ts;
        ts.c_lflag &= ~(ICANON | ECHO); /* 关闭回终端回显功能*/
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts); /* 应用新终端设置 */
    };
    ~VT100() {
        tcsetattr(STDIN_FILENO,TCSAFLUSH, &m_ts); /* 应用新终端设置 */
    }

    void move(int offset, dir_e e) {
        show("\033[%d%c", offset, 'A' + e);
    }
    void push() {
        show("\033[s");
    }
    void pop() {
        show("\033[u");
    }
    void clear_right() {
        show("\033[K");
    }
    static void show(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap);
        fflush(stdout);
        va_end(ap);
    }
 private:
    struct termios m_ts;
};

class shell_cmd {
 public:
    shell_cmd()
        : m_pos(0)
    {    
    }
    ~shell_cmd() {

    };
    bool read_line(string &s) {
        s.assign(m_line.begin(), m_line.end());
        return true;
    }
    size_t pos() {
        return m_pos;
    }
    size_t len() {
        return m_line.size();
    }
    bool clear() {
        m_line.clear();
        m_pos = 0;
    }

    bool get_right(string &s) {
        vector<char>::iterator iter = m_line.begin() + m_pos;
        if (m_line.size() > 0 && iter != m_line.end()) {
            s.assign(iter, m_line.end());
        }

        return true;
    }

    bool shift_right() {
        if (m_pos < m_line.size()) {
            m_pos++;
            return true;
        }
        return false;
    }
    bool shift_left() {
        if (m_pos > 0) {
            m_pos--;
            return true;
        }
        return false;
    }  
    void backspace() {
        // backspace
        if (m_pos == 0) {
            return;
        }        
        vector<char>::iterator iter = m_line.begin() + m_pos - 1;
        m_line.erase(iter);
        m_pos--;
    }
    void del() {
        if (m_pos == m_line.size()) {
            return;
        }
        vector<char>::iterator iter = m_line.begin() + m_pos;
        if (iter != m_line.end()) {
            m_line.erase(iter);
        }
    }
    bool insert(char c) {
        vector<char>::iterator iter = m_line.begin() + m_pos;
        if (m_line.size() > 0 && iter != m_line.end()) {
            m_line.insert(iter, 1, c);
        } else {
            m_line.push_back(c);
        }
        m_pos++;
    }

 private:
    size_t m_pos;
    vector<char> m_line;
};

void start_shell(const char *promt, shell_callback fn)
{
    bool bExit = false;
    VT100 vt;
    shell_cmd sh;

    vt.show("%s", promt);
    while (!bExit) {
        char c = getchar();
        #if 0
        printf("%x\n", c);
        #endif
        switch (c) {
            case 0x8: {
                if (sh.len() == 0) {
                    break;
                }
                string s;
                sh.backspace();                
                sh.get_right(s);
                vt.move(1, VT100::LEFT);
                vt.clear_right();           
                if (!s.empty()) {
                    vt.push();
                    vt.show("%s", s.c_str());
                    vt.pop();
                }
                break;
            }
            case 0x9:
                // tab
                break;
            case 0x1b:
                c = getchar();
                if (c == 0x5b) {
                    c = getchar();
                    switch (c) {
                        case 0x41: // up
                            break;
                        case 0x42: // down
                            break;
                        case 0x43: // right
                            if (sh.shift_right()) {
                                vt.move(1, VT100::RIGHT);
                            }
                            break;
                        case 0x44: // left
                            if (sh.shift_left()) {
                                vt.move(1, VT100::LEFT);
                            }
                            break;
                        case 0x33:
                            c = getchar();
                            if (c == 0x7e) {
                                sh.del();
                                string s;                                    
                                sh.get_right(s);
                                vt.clear_right();
                                vt.push();
                                vt.show("%s", s.c_str());
                                vt.pop();
                            }
                            break;
                    }
                }
                break;
            case 0x0a: {
                vt.show("\n");
                
                string line;
                sh.get_line(line);
                sh.clear();
                if (!line.empty()) {
                    const char **argv = NULL;
                    list<string> lst;
                    str_split(line, lst);
                    if (!lst.empty()) {                    
                        argv = (const char**)malloc(sizeof(char*) * lst.size());
                        size_t argc = 0;
                        list<string>::iterator iter_lst = lst.begin();
                        while (iter_lst != lst.end()) {
                            argv[argc++] = iter_lst->c_str();
                            iter_lst++;
                        }
                    }
                    int ret = fn(lst.size(), argv);
                    if (argv) {
                        free(argv);
                    }
                    if (ret < 0) {
                        return;
                    }
                }
                vt.show("%s", promt);                
                break;
            }
            case 0x0d:
                break;
            default:
                vt.show("%c", c);
                string s;
                sh.get_right(s);
                if (!s.empty()) {
                    vt.clear_right();
                    vt.push();
                }
                sh.insert(c);
                if (!s.empty()) {
                    vt.show("%s", s.c_str());
                    vt.pop();
                }
                break;
        };
    }
}
}
