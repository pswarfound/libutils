#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <list>
#include <iostream>

using std::list;
using std::string;
using std::vector;

void show_promt(const char *p)
{
    printf("%s", p);
    fflush(stdout);
}

// 上移光标  
#define MOVEUP(x) printf("\033[%dA", (x))  
  
// 下移光标  
#define MOVEDOWN(x) printf("\033[%dB", (x))  
  
// 左移光标  
#define MOVELEFT(y) printf("\033[%dD", (y))  
  
// 右移光标  
#define MOVERIGHT(y) printf("\033[%dC",(y))  

#define CLEAR_RIGHT printf("\033[K")
#define SAVE_POS printf("\033[s")
#define REST_POS printf("\033[u")

typedef int (*do_shell)(string &s);
void start_shell(do_shell fn)
{
    bool bExit = false;
    struct termios ts, ots;
    tcgetattr(STDIN_FILENO, &ts);
    ots = ts;
    ts.c_lflag &= ~(ICANON | ECHO); /* 关闭回终端回显功能*/
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&ts); /* 应用新终端设置 */

    vector<char> cmd(256);
    cmd.clear();
    size_t pos = 0;
    vector<char>::iterator iter;
    list<string> history;
    show_promt(">");
    while (!bExit) {
        char c = getchar();

        switch (c) {
            case 0x8: {
                // backspace
                if (pos == 0) {
                    break;
                }
                iter = cmd.begin() + pos - 1;
                cmd.erase(iter);
                pos--;
                string s;
                MOVELEFT(1);
                CLEAR_RIGHT;
                iter = cmd.begin() + pos;
                SAVE_POS;
                s.assign(iter, cmd.end());
                printf("%s", s.c_str());
                fflush(stdout);
                REST_POS;
                break;
            }
            case 0x9:
                // tab
                break;
            case 'q':
                bExit = true;
                break;
            case 0x1b:
                c = getchar();
                if (c == 0x5b) {
                    c = getchar();
                    switch (c) {
                        case 0x41: // up
                            if (history.size() > 0) {

                            }
                            break;
                        case 0x42: // down
                            break;
                        case 0x43: // right
                            if (pos < cmd.size()) {
                                MOVERIGHT(1);
                                pos++;
                            }
                            break;
                        case 0x44: // left
                            if (pos > 0) {
                                MOVELEFT(1);
                                pos--;
                            }
                            break;
                        case 0x33:
                            c = getchar();
                            if (c == 0x7e) {
                                // del                                
                                iter = cmd.begin() + pos;
                                if (iter != cmd.end()) {
                                    cmd.erase(iter);                                
                                    string s;                                    
                                    CLEAR_RIGHT;
                                    iter = cmd.begin() + pos;
                                    if (iter != cmd.end()) {
                                        SAVE_POS;
                                        s.assign(iter, cmd.end());
                                        printf("%s", s.c_str());
                                        fflush(stdout);
                                        REST_POS;
                                    }
                                }
                            }
                            break;
                    }
                }
                break;
            case 0x0a: {
                printf("\n");
                if (!cmd.empty()) {
                    string s;
                    s.assign(cmd.begin(), cmd.end());
                    cmd.clear();
                    pos = 0;
                    if (fn(s) < 0) {
                        return;
                    }
                }
                show_promt(">");
                
                break;
            }
            case 0x0d:
                break;
            default:
                printf("%c", c);
                fflush(stdout);
                iter = cmd.begin() + pos;
                if (cmd.size() > 0 && iter != cmd.end()) {
                    
                    cmd.insert(iter, 1, c);
                    CLEAR_RIGHT;
                    SAVE_POS;
                    string s;
                    s.assign(iter + 1, cmd.end());
                    printf("%s", s.c_str());
                    fflush(stdout);
                    REST_POS;
                } else {
                    cmd.push_back(c);
                }
                pos++;
                break;
        };
    }
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&ots); /* 应用新终端设置 */
}

