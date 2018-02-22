#include "node.h"
#include "symtab.h"
#include "logger.h"
#include <string>
using namespace std;

void Logger::error (string msg) {
    if (*level & LOG_ERROR) {
        fprintf(stderr, "%s[Error] %s\n", prefix.c_str(), msg.c_str());
    }
}

void Logger::error (Location loc, string msg) {
    if (*level & LOG_ERROR) {
        fprintf(stderr, "(%3d:%3d) %s[Error] %s\n", loc.line, loc.column, prefix.c_str(), msg.c_str());
    }
}

void Logger::info (string msg) {
    if (*level & LOG_INFO) {
        fprintf(stderr, "%s[Info ] %s\n", prefix.c_str(), msg.c_str());
    }
}
void Logger::info (Location loc, string msg) {
    if (*level & LOG_INFO) {
        fprintf(stderr, "(%3d:%3d) %s[Info ] %s\n", loc.line, loc.column, prefix.c_str(), msg.c_str());
    }
}
