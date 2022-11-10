#pragma once
#include <iostream>
#include <string>
#include "Oscilloscope.hpp"

class Measurement
{
public:

    Measurement() = default;

    Measurement(std::string name_) : name(name_) { }

    Measurement(std::string stat_response, bool)
    {
        size_t name_start = stat_response.find(" C") + 5;
        size_t name_end = stat_response.find(":");
        name = stat_response.substr(name_start, name_end - name_start);
    }

// private:
    std::string name;
    double mean;
    double min;
    double max;
    double std_dev;
    double count;
};