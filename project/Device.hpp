#pragma once
#include<fstream>
#include<iostream>
#include<string>

class Device 
{
public:
	Device(std::string path_name);

	std::string get_name()
	{
		return name;
	}

protected:
	void command(std::string comm);
	std::string query(std::string comm);

	std::string name;
	std::string root_path;
};



