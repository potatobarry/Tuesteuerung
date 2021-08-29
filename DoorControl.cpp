/*
 * Praktikum MRT2 
 * ART1 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Mai 2018
 * Autor: M.Herhold
 * Version: r2
 */

#include "DoorInterface.h"
#include "DoorControl.h"


DoorControl::DoorControl() : door_if(DoorInterface::get_instance())
{
	//Hallo Lukas
}

DoorControl::~DoorControl()
{
	// destructor
	door_if.quit_doorcontrol_flag = true;
}

void DoorControl::run()
{
	while(!door_if.quit_doorcontrol_flag){
		usleep(20 * 1000);
	}
}
