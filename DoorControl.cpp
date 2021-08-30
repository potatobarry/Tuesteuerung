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
#include "Device.hpp"

DoorControl::DoorControl() : door_if(DoorInterface::get_instance())
{
	Device dev;
}

DoorControl::~DoorControl()
{
	// destructor
	door_if.quit_doorcontrol_flag = true;
}

void DoorControl::run()
{
	//Automatik Modus
	int s = 0;				   //Automatik boot variable
	while (BW1 && BW2 == true) //Automatik Modus
	{
		bool AZ = false; //Admin ZU

		if (s < 1)
		{
			Y3 = true;
			waitms(5000); //warte 5 sekunden
			Y3 = false;
			s++;
		}

		if (NTZ == true)
		{
			AZ = true;
		}

		if ((NTA || LSH || LSV || BM) == true) //Wenn sensor ausgelöst; Lampe an, fahre auf
		{
			Y3 = true;					  //warnlampe an
			while ((ELO || NTZ) == false) //fahre auf, bis endstop erreicht oder bis knopf gedrückt
			{
				Y1 = true;				 //fahre auf
				if (BW1 && BW2 == false) //break bedingung moduswechsel
				{
					Y1 = false;
					Y3 = false;
					break;
				}
			}
			Y1 = false;
			while (int i = 0 < 3000) // warte 3 sekunden, überprüfe jede 0,1 sek pb breakbedingungen erfüllt wurden
			{
				if ((!BW1 || !BW2 || NTZ) == true) //break bedingung moduswechsel
				{
					Y1 = false;
					Y3 = false;
					break;
				}
				waitms(100);
				i = i + 100;
			}
			Y3 = false;
		}

		if ((NTA || LSH || LSV || BM || !AZ) == false) //Fahre zu
		{
			while ((NTA || LSH || LSV || BM || ELG || !BW1 || !BW2)) == false)
				{
					Y2 = true;
					Y3 = true;
				}
			Y2 = false;
			Y3 = false;
			AZ = false;
		}
	}

	//Handbetrieb
	while (BW1 == true && BW2 == false) //Schleife für handbetrieb
	{
		Y1 = false;
		Y2 = false;
		Y3 = false;
		while (NTA == true && NTG == false && !ELO) //Befehl fahre auf (taste auf) und endlage offen nicht erreicht
		{
			Y1 = true;
			Y3 = true;
			if (BW1 && !BW2 == false) //break bedingung moduswechsel
			{
				Y1 = false;
				Y3 = false;
				break;
			}
		}
		while (NTA == false && NTG == true && !ELG) //befehl fahre zu (taste zu gedrückt, endlage nicht erreicht)
		{
			Y2 = true;
			Y3 = true;
			if (BW1 && !BW2 == false) //break bedingung moduswechsel
			{
				Y2 = false;
				Y3 = false;
				break;
			}
		}
	}
	

	while (!door_if.quit_doorcontrol_flag) //all operation modes must be called in this while loop. 
	{
		usleep(20 * 1000);

		DoorInterface &door = DoorInterface::get_instance();
		door.DebugString("Status BW1: " + std::to_string(dev.get_status("BW1")) + "\n");
	}
}