#include "Device.hpp"
#include <vector>
#include <unistd.h>

int main() {

	// initialize generator and oscilloscope
	
	std::string generator_path;
	std::string oscilloscope_path;
	
	for (size_t i = 0; i < 2; i++) {
		auto path = "usbtmc" + std::to_string(i);
		Device device(path);

		if (device.name == "AKIP-3409-4") generator_path = path;
		else if (device.name == "AKIP-4131/1") oscilloscope_path = path;
		else {
			std::cerr << "Devices not available!" << std::endl;
			return EXIT_FAILURE;
		}
	}

	Generator generator(generator_path);
	Oscilloscope oscilloscope(oscilloscope_path);
	

	// work with devices
	
	//generator.buzz();

	std::string waveform;
	std::cout << "Input waveform (SINE, SQUARE, RAMP, PULSE, NOISE, ARB, DC, PRBS, IQ):" << std::endl;
	std::cin >> waveform;

	generator.set_waveform(waveform);
	
	return EXIT_SUCCESS;
}
