#pragma once
#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

class ArgParser {
public:
    ArgParser(int argc, char* argv[]);

    const char* ip() const;
    int port() const;
    bool debug() const;
private:
    void parse(int argc, char* argv[]);

    const char* ip_ = "0.0.0.0";
    int port_ = 42069;
    bool debug_ = false;
};

#endif //!__ARG_PARSER_H__