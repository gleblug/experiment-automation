#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

using port_t = std::size_t;
using ind_t = std::size_t;

enum class channels
{
    C1 = 1,
    C2 = 2
};

std::vector<std::string> files_path(std::string const &path)
{
    size_t file_ind = path.find_last_of('/') + 1;
    std::string directory = path;
    directory.erase(file_ind);

    auto directory_iterator = fs::directory_iterator(directory);
    std::vector<std::string> files_path;

    for (const auto &directory_entry : directory_iterator)
    {
        auto filename = std::string(directory_entry.path());
        std::cout << filename << std::endl;
        if (!filename.find(path))
            files_path.push_back(filename);
    }

    return files_path;
}