#include "arg_parser.h"
#include <cstring>
#include <cstdlib>
#include <cctype>

ArgParser::ArgParser(int argc, char *argv[]) {
    parse(argc, argv);
}

const char* ArgParser::ip() const {
    return ip_;
}

int ArgParser::port() const {
    return port_;
}

bool ArgParser::debug() const {
    return debug_;
}

namespace {
    bool is_number(const char* str) {
        int len = strlen(str);
        for (int i = 0; i < len; ++i) {
            if (!std::isdigit(str[i])) {
                return false;
            }
        }

        return true;
    }

    bool is_ip(const char* str) {
        int len = strlen(str);
        int dots = 0;
        int numbers = 0;
        for (int i = 0; i < len; ++i) {
            if (str[i] == '.') {
                dots++;
            } else if (std::isdigit(str[i])) {
                numbers++;
            } else {
                return false;
            }
        }
        return dots == 3 && numbers >= 4;
    }
}

void ArgParser::parse(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (std::strcmp(argv[i], "--debug") == 0) {
            debug_ = true;
        } else if (is_ip(argv[i])) {
            ip_ = argv[i];
        } else if (is_number(argv[i])) {
            port_ = atoi(argv[i]);
        }
    }
}
