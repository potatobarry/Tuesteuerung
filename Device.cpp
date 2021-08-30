#include "Device.hpp"
#include "CSV.hpp"
#include "DoorInterface.h"

Device::Device()
{
	CSV csv("config.csv");
	device = csv.get_devices();
}

bool Device::get_status(std::string sensor) //should work? PLEASE TEST
{
	DoorInterface& door = DoorInterface::get_instance();
	
	unsigned int port = device[sensor].port;
	unsigned char pin;
	bool status;
	door.DIO_Read(port, &pin);
	
	if (device[sensor].active_state) {
		status = (pin >> device[sensor].pin) & 1;
	}
	else {
		status = !((pin >> device[sensor].pin) & 1);	//flips boolean value for devices with active state = 0
	}
		
	return status;
}

void Device::set_status(std::string actor, int status)
{
	unsigned int port = device[actor].port;
	unsigned char pin = 0;
}
