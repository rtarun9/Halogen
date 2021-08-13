#pragma once

#include "log.h"

#include <string>
#include <fstream>
#include <vector>

namespace halogen
{
    enum class FileType
    {
        Shader = 0,
        Obj = 1,
        Mtl = 2
    };

    //[WIP] : Automatically finds the relative path to the specified file path
    //Example : Rather than ../../assets/obj_file.obj, do assets/obj_file.obj, and the function will add the ../../
    //Not used for now, but will be used when more file types are supported.
    static std::string file_system(std::string& file_path, FileType file_type)
	{
    	std::string relative_path;

    	switch (file_type)
		{
			case FileType::Shader :
			{
				//Currently .spv's are in same directory as bin. May change later.
				relative_path = std::string("") + relative_path;
				break;
			}

			case FileType::Obj :
			{
				relative_path = std::string("../") + relative_path;
				break;
			}

			case FileType::Mtl :
			{
				relative_path = std::string("../assets/") + relative_path;
				break;
			}

			default :
			{
				debug::warning("Unsupported file type requested.");
				break;
			}
		}

		return file_path;
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

        halogen::ASSERT(file_buffer_size == 0, "Cannot read shader : size is equal to 0.");

        std::vector<char> file_contents(file_buffer_size);

        file.seekg(0);
        file.read(file_contents.data(), file_buffer_size);

        file.close();
        return file_contents;
    }
}
