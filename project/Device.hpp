#include<fstream>
#include<iostream>
#include<string>

class Device {
public:
	Device(std::string path_name): root_path("/dev/" + path_name) {
		std::string idn = query("*IDN?");
		idn.erase(0, idn.find(',') + 1);
		idn.erase(idn.find(','));

		name = idn;
	}

protected:
	void command(std::string comm) {
		std::ofstream out;
		out.open(root_path);

		if(!out.is_open())
			std::cerr << "**ERROR** file is not open to output." << std::endl;
		else {
			out << comm;
			out.close();
		}
	}

	std::string query(std::string comm) {
		command(comm);
		
		std::string buffer;
		std::ifstream in;
		in.open(root_path);

		if(!in.is_open()) 
			std::cerr << "**ERROR** file is not open to input." << std::endl;
		else {
			std::getline(in, buffer);
			in.close();
		}

		return buffer;
	}

public:
	std::string name;
	std::string root_path;
};


class Generator: public Device {
public:
	Generator(std::string path_name): Device(path_name), channel("C1") {  }

	void buzz() {
		command("BUZZ ON");
	}

	void basic_wave(
		size_t channel,
		std::string type,
		double frequency,
		double period,
		double amplitude,
		double offset,
		double phase
	);

	void set_channel(size_t ch) {
		if (ch == 1 || ch == 2) channel = "C" + std::to_string(ch);
		else std::cerr << "Invalid channel" << std::endl;
	}

	void set_waveform(std::string type) {
		command(channel + ":BSWV WVTP," + type);
	}

private:
	std::string channel;
};


class Oscilloscope: public Device {
public:
	Oscilloscope(std::string path_name): Device(path_name) {  }
};








