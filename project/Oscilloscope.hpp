#pragma once
#include "Device.hpp"

class Oscilloscope : public Device
{
public:
    Oscilloscope(std::string path_name) : Device(path_name) {}
};