#pragma once
class Device
{
	int port;
	int pin;
	int active_state;
public: 
	int get_state();

};