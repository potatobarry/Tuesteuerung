#include "Device.hpp"
#include "CSV.hpp"
#include "DoorInterface.h"

Device::Device()
{
	CSV csv("config.csv");
	device = csv.get_devices();
	actorpins = 0;
}

bool Device::get_status(std::string sensor)
{
	DoorInterface& door = DoorInterface::get_instance();

	unsigned int port = device[sensor].port;
	unsigned char pins;		//will contain entire byte of pin information for port
	bool status;
	door.DIO_Read(port, &pins);

	if (device[sensor].active_state) {
		status = (pins >> device[sensor].pin) & 1;		//gives us state of bit N = device[sensor].pin
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

	if (!device[actor].active_state)	//inverts wanted status for hypothetical actors with inverted active states
	{
		status = !status;
	}

	//converts string actor and bool status to appropriate pins byte
	if (status)
	{
		actorpins |= 1 << device[actor].pin;		//sets appropriate bit to 1
	}
	else
	{
		actorpins &= ~(1 << device[actor].pin);		//sets appropriate bit to 0
	}

	door.DIO_Write(port, actorpins);
}

void Device::door_open()
{
	this->set_status("Y2", 0);
	//this->set_status("Y3", 0);
	this->set_status("Y1", 1);
}

void Device::door_close()
{
	this->set_status("Y1", 0);
	this->set_status("Y2", 1);
	//this->set_status("Y3", 1);
}

void Device::door_stop()
{
	this->set_status("Y1", 0);
	this->set_status("Y2", 0);
}

void Device::lamp(int status)
{
	this->set_status("Y3", status);
}
