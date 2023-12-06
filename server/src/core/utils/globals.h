#pragma once
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <filesystem>

namespace globals {
    inline const auto executable_path = std::filesystem::read_symlink("/proc/self/exe");

    inline const auto executable_dir = std::filesystem::read_symlink("/proc/self/exe").parent_path();

    inline bool debug = false;
}
#endif //!__GLOBALS_H__