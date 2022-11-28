#pragma once

#include <string>
#include <iostream>

#include "Utils.hpp"


class Generator
{
public:
    Generator();

    void command(std::string const &comm) const;
    std::string query(std::string const &comm) const;

    void set_channel(channels ch)
    {
        m_channel = "C" + std::to_string(static_cast<ind_t>(ch));
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
    std::string m_root_path;
    std::string m_channel;
};