#include <fstream>
#include "CSV.hpp"
#include "DoorInterface.h"

CSV::CSV(std::string path) : path(path) {
	read_file();
}

std::list<std::string> CSV::parse_line(std::string line) {
	DoorInterface::get_instance().DebugString(line);
	std::list<std::string> list;
	return list;
}

void CSV::read_file() {
	std::ifstream stream(path);
	std::string line;

	while (std::getline(stream, line)) {
		if (line.rfind("#", 0) == 0)
			continue;
		else
			auto data = parse_line(line);
	}
}

std::map<std::string, Device> CSV::get_devices() {
	return devices;
}