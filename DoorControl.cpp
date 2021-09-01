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

void DoorControl::automatik()
{
	int s = 0;													   //Automatik boot variable
	while (dev.get_status("BW1") && dev.get_status("BW2") == true) //Automatik Modus
	{
		bool az = false; //Admin ZU

		if (!s)
		{
			dev.lamp(1);
			usleep(5000 * 1000); //warte 5 sekunden
			dev.lamp(0);
			s++;
		}

		if (dev.get_status("NTZ"))
		{
			az = true; //administration zu
		}

		if (dev.get_status("NTA") || dev.get_status("LSH") || dev.get_status("LSV") || dev.get_status("BM")) //Wenn sensor ausgelöst; Lampe an, fahre auf
		{
			dev.lamp(1);											  //warnlampe an
			while (!(dev.get_status("ELO") || dev.get_status("NTZ"))) //fahre auf, bis endstop erreicht oder bis knopf gedrückt
			{
				dev.door_open();									   //fahre auf
				if (!(dev.get_status("BW1") && dev.get_status("BW2"))) //break bedingung moduswechsel
				{
					dev.door_stop();
					dev.lamp(0);
					break;
				}
			}
			dev.door_stop();
			while (int i = 0 < 3000) // warte 3 sekunden, überprüfe jede 0,1 sek pb breakbedingungen erfüllt wurden
			{
				if ((!dev.get_status("BW1") || !dev.get_status("BW2") || dev.get_status("NTZ"))) //break bedingung moduswechsel
				{
					dev.door_stop();
					dev.lamp(0);
					break;
				}
				usleep(100 * 1000);
				i = i + 100;
			}
		}

		if (!(dev.get_status("NTA") || dev.get_status("LSH") || dev.get_status("LSV") || dev.get_status("BM") || az)) //Fahre zu
		{
			dev.door_close();
			dev.lamp(1);
			while (!(dev.get_status("NTA") || dev.get_status("LSH") || dev.get_status("LSV") || dev.get_status("BM") || dev.get_status("ELG") || !dev.get_status("BW1") || !dev.get_status("BW2")))
			{
				usleep(100 * 1000); //warte 100 ms
			}
			dev.door_stop();
			dev.lamp(0);
			az = false;
		}
	}
}

void DoorControl::handbetrieb()
{
	dev.door_stop();
	dev.door_stop();
	dev.lamp(0);
	while (dev.get_status("NTA") == true && dev.get_status("NTG") == false && !dev.get_status("ELO")) //Befehl fahre auf (taste auf) und endlage offen nicht erreicht
	{
		dev.door_open();
		dev.lamp(1);
		if (dev.get_status("BW1") && !dev.get_status("BW2") == false) //break bedingung moduswechsel
		{
			dev.door_stop();
			dev.lamp(0);
			break;
		}
	}
	while (dev.get_status("NTA") == false && dev.get_status("NTG") == true && !dev.get_status("ELG")) //befehl fahre zu (taste zu gedrückt, endlage nicht erreicht)
	{
		dev.door_close();
		dev.lamp(1);
		if (dev.get_status("BW1") && !dev.get_status("BW2") == false) //break bedingung moduswechsel
		{
			dev.door_stop();
			dev.lamp(0);
			break;
		}
	}
}

void DoorControl::reperaturbetrieb()
{

	if (dev.get_status("NTA") || dev.get_status("LSV") || dev.get_status("LSH") || dev.get_status("BM") || !dev.get_status("ELO")) //fahre auf
	{
		while (dev.get_status("NTA") || dev.get_status("LSV") || dev.get_status("LSH") || dev.get_status("BM") || !dev.get_status("ELO"))
		{
			dev.door_open();
			dev.lamp(1);
			if (!dev.get_status("ELO") || dev.get_status("NTZ"))
			{
				break;
			}
		}
		dev.door_stop(); //motor stop und licht aus

		while (int i = 0 < 3000) // warte 3 sekunden, überprüfe jede 0,1 sek pb breakbedingungen erfüllt wurden
		{
			if (dev.get_status("NTZ") || !(!dev.get_status("BW1") && dev.get_status("BW2"))) //break bedingung moduswechsel
			{
				dev.door_stop();
				dev.lamp(0);
				break;
			}
			usleep(100 * 1000);
			i = i + 100;
		}
	}
	if (!(dev.get_status("NTA") || dev.get_status("LSV") || dev.get_status("LSH") || dev.get_status("BM") || !dev.get_status("ELG"))) //fahre zu
	{
		while (dev.get_status("NTZ"))
		{
			dev.door_close();
			if (dev.get_status("ELG") || !(!dev.get_status("BW1") && dev.get_status("BW2")))
			{
				break;
			}
		}
		dev.door_stop();
	}
}

void DoorControl::ausgeschaltet()
{
	usleep(100 * 1000); //warte 100ms
}

void DoorControl::run()
{
	while (!door_if.quit_doorcontrol_flag) //all operation modes must be called in this while loop.
	{
		usleep(20 * 1000);

		//	DoorInterface &door = DoorInterface::get_instance();
		//	door.DebugString("Status BW1: " + std::to_string(dev.get_status("BW1")) + "\n");
		//Automatik Modus

		automatik();

		//Handbetrieb
		while (dev.get_status("BW1") && !(dev.get_status("BW2"))) //Schleife für handbetrieb
		{
			handbetrieb();
		}

		while (!(dev.get_status("BW1")) && dev.get_status("BW2"))
		{
			reperaturbetrieb();
		}
		while (!(dev.get_status("BW1")) && !(dev.get_status("BW2")))
		{
			ausgeschaltet();
		}
	}
}

