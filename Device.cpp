#include "Device.hpp"
#include "CSV.hpp"
#include "DoorInterface.h"

Device::Device()
{
	CSV csv("config.csv");
	device = csv.get_devices();
}

bool Device::get_status(std::string sensor)
{
	DoorInterface& door = DoorInterface::get_instance();
	
	unsigned int port = device[sensor].port;
	unsigned char pins;
	bool status;
	door.DIO_Read(port, &pins);
	
	if (device[sensor].active_state) {
		status = (pins >> device[sensor].pin) & 1;
	}
	else {
		status = !((pins >> device[sensor].pin) & 1);	//flips boolean value for devices with active state = 0
	}
		
	return status;
}

void Device::set_status(std::string actor, bool status)
{
	DoorInterface& door = DoorInterface::get_instance();
	
	unsigned int port = device[actor].port;
	unsigned char pins;
	door.DIO_Read(port, &pins);	//gets current actor states
	
	if (!device[actor].active_state)	//inverts wanted status for hypothetical actors with inverted active states
	{
		status = !status;
	}

	//converts string actor and bool status to appropriate pins byte
	if (status)
	{
		pins |= 1 << device[actor].pin;
	}
	else
	{
		pins &= ~(1 << device[actor].pin);
	}

	door.DIO_Write(port, pins);	//invertiert die kacke
}
