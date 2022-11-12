#pragma once
#include "Device.hpp"

class Generator : public Device
{
public:
    Generator(std::string path_name) : Device(path_name), m_channel("C1") {}

    void set_channel(size_t channel)
    {
        if (channel == 1 || channel == 2)
            m_channel = "C" + std::to_string(channel);
        else
            std::cerr << "Invalid channel" << std::endl;
    }

    void buzz()
    {
        command("BUZZ ON");
    }

    void set_waveform(std::string waveform)
    {
        command(m_channel + ":BSWV WVTP," + waveform);
    }

    void set_frequency(double frequency)
    {
        command(m_channel + ":BSWV FRQ," + std::to_string(frequency));
    }

    void set_period(double period)
    {
        command(m_channel + ":BSWV PERI," + std::to_string(period));
    }

    void set_amplitude(double amplitude)
    {
        command(m_channel + ":BSWV AMP," + std::to_string(amplitude));
    }

    void set_offset(double offset)
    {
        command(m_channel + ":BSWV OFST," + std::to_string(offset));
    }

    void set_phase(double phase)
    {
        command(m_channel + ":BSWV PHSE," + std::to_string(phase));
    }

private:
    std::string m_channel;
};