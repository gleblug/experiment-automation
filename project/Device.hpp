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
	Generator(std::string path_name): Device(path_name), m_channel("C1") {  }

	void set_channel(size_t channel) {
		if (channel == 1 || channel == 2) m_channel = "C" + std::to_string(channel);
		else std::cerr << "Invalid channel" << std::endl;
	}

	void buzz() {
		command("BUZZ ON");
	}

	void set_waveform(std::string waveform) {
		m_waveform = waveform;
		command(m_channel + ":BSWV WVTP," + waveform);
	}

	void set_frequency(double frequency) {
		m_frequency = frequency;
		command(m_channel + ":BSWV FRQ," + std::to_string(frequency));
	}

	void set_period(double period) {
		command(m_channel + ":BSWV PERI," + std::to_string(period));
	}

	void set_amplitude(double amplitude) {
		command(m_channel + ":BSWV AMP," + std::to_string(amplitude));
	}

	void set_offset(double offset) {
		command(m_channel + ":BSWV OFST," + std::to_string(offset));
	}

	void set_phase(double phase) {
		command(m_channel + ":BSWV PHSE," + std::to_string(phase));
	}

	void update();

private:
	std::string m_channel;
	std::string m_waveform;
	double m_frequency;
	double m_period;
	double m_amplitude;
	double m_offset;
	double m_phase;
};


class Oscilloscope: public Device {
public:
	Oscilloscope(std::string path_name): Device(path_name) {  }
};








