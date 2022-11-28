#include "Device.hpp"


Device::Device(std::string path_name) : root_path("/dev/" + path_name)
{
    std::string response = query("*IDN?");
    response.erase(0, response.find(',') + 1);
    response.erase(response.find(','));

    name = response;
}

void Device::command(std::string const &comm) const
{
    std::ofstream out;
    out.open(root_path);

    if (!out.is_open())
        std::cerr << "**ERROR** file is not open to output." << std::endl;
    else
    {
        out << comm;
        out.close();
    }
}

std::string Device::query(std::string const &comm) const
{
    command(comm);

    std::string buffer;
    std::ifstream in;
    in.open(root_path);

    if (!in.is_open())
        std::cerr << "**ERROR** file is not open to input." << std::endl;
    else
    {
        std::getline(in, buffer);
        in.close();
    }

    return buffer;
}
