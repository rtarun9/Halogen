#ifndef COMMON_H
#define COMMON_H

#include "log.h"

#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <functional>

// This file will be filled with useful things later on, pretty barren for now.

namespace halogen
{
    namespace Time
    {
        constexpr long One_Second = 1000000000;
    }

    //Read binary files for shaders.
    static std::vector<char> read_binary_file_from_path(const char *file_path)
    {
        /* relative_file_path will be useful if we change directory paths. */
        std::string relative_file_path = std::string(file_path);
        std::ifstream file = std::ifstream(relative_file_path, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            halogen::debug::error("Could not open file !!", "Given path : ", file_path);
        }
        size_t file_buffer_size = static_cast<size_t>(file.tellg());

        //halogen::debug::error(file_buffer_size == 0, "Invalid file, size is less than 0.");

        std::vector<char> file_contents(file_buffer_size);

        file.seekg(0);
        file.read(file_contents.data(), file_buffer_size);

        file.close();
        return file_contents;
    }

    //TODO : Move to a vk_types file or something similar.
    //Destruction queue : Used for deletion of vulkan objects ONLY
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function)
        {
            deletors.push_back(function);
        }

        //This will call the lambda functions in the same order that they were queued in. (since its a double ended queue, it follows the FIFO principle).
        //Not a very efficient way, but gets the job done.

        void flush()
        {
            //Using rbegin and rend here since we want to iterate over the double ended queue in reverse, because
            //Usually objects in vulkan are deleted in the reverse order they were implemended.
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                //Dereference the function and call them.
                (*it)();
            }

            deletors.clear();
        }
    };
}

#endif