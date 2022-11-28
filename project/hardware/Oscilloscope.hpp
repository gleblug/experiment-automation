#pragma once
#include <iostream>
#include <string>
#include <exception>
#include <fstream>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "Utils.hpp"

class Oscilloscope
{
public:
    Oscilloscope(std::string server_ip = "10.11.13.220", port_t server_port = 5024);

    size_t command(std::string const &comm)
    {
        return send(m_client_side, comm.c_str(), comm.length(), 0);
    }

    std::string request();
    size_t request(char *buf, size_t msg_size);
    std::string query(std::string const& comm);

    void set_channel(channels ch)
    {
        m_channel = "C" + std::to_string(static_cast<ind_t>(ch));
    }

    double parser(std::string const& comm, std::string const& to_find, std::string const& units);

    double get_vdiv()
    {
        return parser(m_channel + ":VDIV?", "VDIV", "V");
    }

    double get_voffset()
    {
        return parser(m_channel + ":OFST?", "OFST", "V");
    }

    double get_timebase()
    {
        return parser("TDIV?", "TDIV", "S");
    }

    double get_sampling_rate()
    {
        return parser("SARA?", "SARA", "Sa/s");
    }

    void save_waveform(std::string file_name = "waveform.csv");

    ~Oscilloscope();

private:
    int m_client_side;
    std::string m_channel;
};