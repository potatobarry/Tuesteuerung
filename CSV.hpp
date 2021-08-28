#pragma once

#include <map>
#include <list>
#include <string>

struct Device {
	int pin;
	int port;
	int active_state;
	std::string name;
	std::string type;
};

class CSV
{
	std::string path;
	std::map<std::string, Device> devices;
	std::list<std::string> parse_line(std::string);
	void read_file();
	public:
		CSV(std::string path);
		std::map<std::string, Device> get_devices();
};
