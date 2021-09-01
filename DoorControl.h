/*  -*- mode: c++; -*-

 * Praktikum MRT2
 * ART1 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Mai 2018
 * Autor: M.Herhold
 * Version: r3
 */

#ifndef DOORCONTROL_HH
#define DOORCONTROL_HH

#include "ncurses.h"
#include "DoorInterface.h"
#include "Device.hpp"


class DoorControl {
	Device dev;
public:
	DoorControl();
	~DoorControl();

	void run();
private:
	DoorInterface& door_if;
	void automatik();
	void handbetrieb();
	void reperaturbetrieb();
	void ausgeschaltet();
};


#endif // DOORCONTROL_HH
