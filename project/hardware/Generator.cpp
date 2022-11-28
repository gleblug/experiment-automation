#include "Generator.hpp"

#include <fstream>

Generator::Generator() : m_channel("C1")
{
    std::string response = query("*IDN?");
    response.erase(0, response.find(',') + 1);
    response.erase(response.find(','));

    // name = response;
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