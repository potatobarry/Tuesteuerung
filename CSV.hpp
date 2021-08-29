#pragma once

#include <map>
#include <vector>
#include <string>

struct Config {
	int port;
	int pin;
	std::string type;
	int active_state;
};

class CSV
{
	std::string path;
	std::map<std::string, Config> devices;
	std::vector<std::string> parse_line(std::string);
	void read_file();
	public:
		CSV(std::string path);
		std::map<std::string, Config> get_devices();
};
