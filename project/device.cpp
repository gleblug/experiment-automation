#include<fstream>
#include<iostream>
#include<string>

class Device
{
public:

  Device(std::string name): root_path("/dev/" + name) {  }

  void send_command(std::string command)
  {
    std::ofstream out;
    out.open(root_path);

    if(!out.is_open()) {
    std::cerr << "**ERROR** file is not open to output." << std::endl;
    } else {
    out << command;
    out.close();
    }
  }

  std::string accept_command()
  {
    std::ifstream in;
    in.open(root_path);

    if(!in.is_open()) {
    std::cerr << "**ERROR** file is not open to input." << std::endl;
    } else {
    std::getline(in, buffer);
    in.close();
    }

    return buffer;
  }

private:
  std::string root_path;
  std::string buffer;
};

int main()
{
  Device generator("usbtmc0");
  generator.send_command("*IDN?");

  std::cout << generator.accept_command() << std::endl;

  return 0;
}
