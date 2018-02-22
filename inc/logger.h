#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "node.h"
#include "symtab.h"
#include <string>
using namespace std;

#define LOG_INFO 0x100
#define LOG_WARNING 0x010
#define LOG_ERROR 0x001

#define LOG_LEVEL_INFO 0x111
#define LOG_LEVEL_WARNING 0x011
#define LOG_LEVEL_ERROR 0x001

class Logger {
private:
    string prefix;
    int *level;
public:
    void error (string msg);
    void error (Location loc, string msg);
    void info (string msg);
    void info (Location loc, string msg);
    // void warning(Location loc, string msg);

    Logger(string s = "", int *n = NULL): prefix(s), level(n){}
};

#endif
