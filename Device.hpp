#pragma once
#include <map>
#include "CSV.hpp"

class Device
{
	std::map<std::string, Config> device;
	
public: 
	Device();
	bool get_status(std::string sensor);
	void set_status(std::string actor, bool status);

};