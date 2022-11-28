#pragma once
#include "Device.hpp"
#include "Measurement.hpp"
#include <string>
#include <unistd.h>
#include <exception>
#include <cassert>

class Oscilloscope : public Device
{
public:
    Oscilloscope(std::string path_name) : Device(path_name), m_channel("C1") {}

    void set_channel(unsigned int channel)
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
        
        std::string response = quer_param_value("STAT1");
        comm_param_stat(0);

        std::cout << response << std::endl;
        return Measurement(response, 1);
    }

    void get_waveform()
    {
        command(m_channel + ":WF? DAT2");

        usleep(1e7);

        std::ifstream in;
        std::ofstream out;
        out.open("out.txt", std::ios_base::trunc | std::ios_base::binary | std::ios_base::out);
        in.open(root_path, std::ios_base::binary);
        // in.open(root_path, std::ios_base::binary | std::ios_base::in);

        // if (!in.is_open())
        //     throw std::runtime_error("File is not enable to write!");
        
        size_t buf_size = 1400026;
        size_t cur_size = 0;
        size_t upd = 0;
        char *buffer = new char [buf_size];

        while (in)
        {
            in.get(buffer + cur_size, buf_size);
            cur_size += in.gcount();
            if (upd != cur_size)
            {
                out.write(buffer + upd, cur_size - upd);
                std::cout << cur_size << std::endl;
                upd = cur_size;
            }
        }
        // std::cout << in.gcount() << std::endl;
        // char response = '0';
        // while (response != '#')
        //     in >> response;

        // char count_of_integers;
        // in >> count_of_integers;
        // size_t coi = count_of_integers - 48; 

        // assert(coi == 9);

        // char *count_of_bytes = new char [coi];
        // for (size_t i = 0; i < coi; i++)
        //     in >> count_of_bytes[i];
        // size_t cob = std::stoi(count_of_bytes);

        // signed char *data = new signed char [cob];
        // for (size_t i = 0; i < cob; i++)
        //     in >> data[i];

        // char endl;
        // in >> endl;
        // // assert(endl == 10);

        // std::cout << cob << std::endl;
        // std::cout << data[0] << std::endl;
        in.close();
        out.close();
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