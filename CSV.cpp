#include <fstream>
#include "CSV.hpp"
#include "DoorInterface.h"

CSV::CSV(std::string path) : path(path) {
	read_file();
}

std::vector<std::string> CSV::parse_line(std::string line) {
	DoorInterface& door = DoorInterface::get_instance(); //makes pointer to singleton object door
	door.DebugString(line);	//writes current line to log file
	std::vector<std::string> devprops;	//creates vector for all properties of current lines device: vector is an array with variable size

	int prevcomma = -1;	//previous comma
	for (unsigned int i = 0; i < line.length(); i++)	//takes current line and goes thru each symbol
	{
		std::string str = std::string(1, line[i]); //creates string from current symbol in active line
		if (str == ",") {
			devprops.push_back(line.substr(prevcomma + 1, i - prevcomma - 1)); //adds substring between commas to end of vector
			prevcomma = i;	//sets previous comma to current
		}
	}
	//for (std::string str : devprops) //outputs each thing to log file
	//{
	//	door.DebugString(str);
	//}
	return devprops;
}

void CSV::read_file() {
	std::ifstream stream(path);
	std::string line;

	while (std::getline(stream, line)) {
		std::vector<std::string> data;

		if (line.rfind("#", 0) == 0)		// if first symbold is # ignores line. TODO: any other unexpected line causes segfault! Please fix!
			continue;
		else
			data = parse_line(line);	//sends current line to parse_line()

		Config dev;	//struct with properties of device
		dev.port = std::stoi(data[1]);
		dev.pin = std::stoi(data[2]);
		dev.type = data[3];
		dev.active_state = std::stoi(data[4]);

		devices[data[0]] = dev;	//fills devices map with key = name of sensor/actor and its properties
	}
}

std::map<std::string, Config> CSV::get_devices() {
	return devices;
}