#pragma once
#include <map>
#include "CSV.hpp"

class Device
{
	unsigned char actorpins;
	std::map<std::string, Config> device;
	
public: 
	Device();
	//get_status function returns bool of current status. Call by inserting case sensitive name of sensor in brackets. Anything not on ports 0 and 1 outputs random numbers. 
	bool get_status(std::string sensor);
	//set_status function sets state of actor called before comma (case sensitive string!) to boolean value inserted after comma. 
	void set_status(std::string actor, bool status);

};