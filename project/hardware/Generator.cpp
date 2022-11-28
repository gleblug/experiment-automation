#include "Generator.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Generator::Generator() : m_channel("C1")
{
    auto devices_path = files_path("/dev/usbtmc");
    bool success = false;

    for (auto const &device_path : devices_path)
    {
        m_root_path = device_path;
        std::string response = query("*IDN?");
        response.erase(0, response.find(',') + 1);
        response.erase(response.find(','));

        if (response == "AKIP-3409-4")
        {
            success = true;
            break;
        }
    }

    if (!success)
        throw std::runtime_error("Generator (AKIP-3409-4) is not available!");
}

void Generator::command(std::string const &comm) const
{
    std::ofstream out;
    out.open(m_root_path);

    if (!out.is_open())
        std::cerr << "**ERROR** file is not open to output." << std::endl;
    else
    {
        out << comm;
        out.close();
    }
}

std::string Generator::query(std::string const &comm) const
{
    command(comm);

    std::string buffer;
    std::ifstream in;
    in.open(m_root_path);

    if (!in.is_open())
        std::cerr << "**ERROR** file is not open to input." << std::endl;
    else
    {
        std::getline(in, buffer);
        in.close();
    }

    return buffer;
}