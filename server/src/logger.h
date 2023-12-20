#pragma once
#ifndef __LOG_H__
#define __LOG_H__

#include <color.hpp>
#include "core/utils/globals.h"
#include <iostream>
#include <string_view>
#include <chrono>
#include <mutex>

class Logger {
public:
    class LogEntry;
    class LogEntryDebug;
    
    inline Logger(const Logger&) = delete;

    inline Logger& operator=(const Logger&) = delete;

    inline Logger(std::ostream& os)
        : os_(os)
    { }

    inline LogEntry info() {
        return LogEntry(*this, "info", cc::darkgreen);
    }

    inline LogEntry warning() {
        return LogEntry(*this, "warn", cc::darkyellow);
    }

    inline LogEntry error() {
        return LogEntry(*this, "fatal", cc::red);
    }

    inline LogEntryDebug debug() {
        return LogEntryDebug(*this);
    }

private:
    std::ostream& os_;
    std::mutex mutex_;

public:
    class LogEntry {
    public:
        inline LogEntry(Logger& logger, const std::string& prefix, const jaszyk::ConsoleColor& color)
            : logger_(logger), lock_(logger.mutex_)
        {
            logger_.os_ << cc::white << '[' << color << prefix << cc::white << "] " << cc::reset;
        }

        inline LogEntry(const LogEntry&) = delete;

        inline LogEntry& operator=(const LogEntry&) = delete;

        inline ~LogEntry() {
            logger_.os_ << std::endl;
        }

        template <typename T>
        inline LogEntry& operator<<(const T& value) {
            logger_.os_ << value;
            return *this;
        }

        inline LogEntry& operator<<(std::ostream& (*fn)(std::ostream&)) {
            logger_.os_ << fn;
            return *this;
        }


    private:
        Logger& logger_;
        std::lock_guard<std::mutex> lock_;
    };

    class LogEntryDebug {
    public:
        inline LogEntryDebug(Logger& logger)
            : logger_(logger), lock_(logger.mutex_)
        {
            if (globals::debug) logger_.os_ << cc::white << '[' << cc::blue << "debug" << cc::white << "] " << cc::reset;
        }

        inline LogEntryDebug(const LogEntryDebug&) = delete;

        inline LogEntryDebug& operator=(const LogEntryDebug&) = delete;

        inline ~LogEntryDebug() {
            if (globals::debug) logger_.os_ << std::endl;
        }

        template <typename T>
        inline LogEntryDebug& operator<<(const T& value) {
            if (globals::debug) logger_.os_ << value;
            return *this;
        }

        inline LogEntryDebug& operator<<(std::ostream&(*fn)(std::ostream&)) {
            if (globals::debug) logger_.os_ << fn;
            return *this;
        }
    private:
        Logger& logger_;
        std::lock_guard<std::mutex> lock_;
    };
};

#if LOG_TO_FILE == 1
#include <fstream>

inline std::ofstream _log_file("log.txt", std::ios::app);
inline Logger logger{_log_file};
#else
inline Logger logger{std::cout};
#endif

#endif //!__LOG_H__