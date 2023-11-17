#pragma once
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <filesystem>

const auto executable_path = std::filesystem::read_symlink("/proc/self/exe");

const auto executable_dir = std::filesystem::read_symlink("/proc/self/exe").parent_path();

#endif //!__GLOBALS_H__