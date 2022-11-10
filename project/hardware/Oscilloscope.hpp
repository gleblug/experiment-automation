#pragma once
#include "Device.hpp"
#include "Measurement.hpp"
#include <string>
#include <unistd.h>

class Oscilloscope : public Device
{
public:
    Oscilloscope(std::string path_name) : Device(path_name), m_channel("C1") {}

    void set_channel(size_t channel)
    {
        if (channel == 1 || channel == 2)
            m_channel = "C" + std::to_string(channel);
        else
            std::cerr << "Invalid channel" << std::endl;
    }

    Measurement get_pkpk()
    {
        comm_param_custom("PKPK");
        comm_param_stat(1);

        // size_t count = 0;
        // while (count <= 20)
        // {
        //     std::string response = quer_param_value("STAT1");
        //     size_t count_ind = response.find("count,") + 5;
        //     std::string count_str = response.substr(count_ind);
        //     std::cerr << count_str;
        //     comm_param_stat(0);

        //     count = std::stoi(count_str);
        // }
        std::string response = quer_param_value("STAT1");
        comm_param_stat(0);

        std::cout << response << std::endl;
        return Measurement(response, 1);
    }


private:
    void comm_param_custom(std::string param)
    {
        command("PACU " + param + "," + m_channel);
    }

    void comm_param_stat(bool condition)
    {
        command("PASTAT " + condition ? "ON" : "OFF");
    }

    std::string quer_param_value(std::string custom)
    {
        return query("PAVA? " + custom);
    }

    std::string m_channel;
};