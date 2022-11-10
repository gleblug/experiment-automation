#include "Generator.hpp"
#include "Oscilloscope.hpp"
#include <vector>
#include <unistd.h>

int main() {

	// initialize generator and oscilloscope
	
	std::string generator_path;
	std::string oscilloscope_path;
	
	for (size_t i = 0; i < 2; i++) {
		auto path = "usbtmc" + std::to_string(i);
		Device device(path);

		if (device.get_name() == "AKIP-3409-4")
			generator_path = path;
		else if (device.get_name() == "AKIP-4131/1")
			oscilloscope_path = path;
		else {
			std::cerr << "Devices not available!" << std::endl;
			return EXIT_FAILURE;
		}
	}
	
	Generator generator(generator_path);
	Oscilloscope oscilloscope(oscilloscope_path);
	

	// work with devices
	
	std::cout << oscilloscope.query("C1:WF? DAT2").substr(1, 3) << std::endl;
	
	return EXIT_SUCCESS;
}
