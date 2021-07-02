#ifndef COMMON_H
#define COMMON_H

#include "log.h"

#include <string>
#include <fstream>
#include <vector>

/* This file will be filled with useful things later on, pretty barren for now. */

static std::vector<char> read_binary_file_from_path(const char *file_path)
{
    /* relative_file_path will be useful if we change directory paths. */
    std::string relative_file_path = std::string(file_path);
    std::ifstream file = std::ifstream(relative_file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        halogen::debug::log("Could not open file !!");
        halogen::ASSERT(true, file_path);
    }
    size_t file_buffer_size = static_cast<size_t>(file.tellg());

    std::vector<char> file_contents(file_buffer_size);

    file.seekg(0);
    file.read(file_contents.data(), file_buffer_size);

    file.close();
    return file_contents;
}
#endif