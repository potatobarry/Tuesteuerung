#include "DoorControl.h"
#include "CSV.hpp"
#include "DoorInterface.h"

/* Das Programm in Eclipse mittels des Menüeintrags:
 * "Run -> External Tools" -> run in xterm"
 * starten! */
int main (int argc, char *argv[])
{
	//CSV csv = CSV("config.csv");
	//auto devices = csv.get_devices();
	DoorInterface& door = DoorInterface::get_instance(); //makes pointer to singleton object door

	//for (auto it : devices)
	//{
	//	door.DebugString(it.first + "\n" + std::to_string(it.second.port));
	//	door.DebugString(std::to_string(it.second.pin));
	//	door.DebugString(std::to_string(it.second.active_state));
	//	door.DebugString(it.second.type);
	//}

	DoorControl control;
	control.run();

	return 0;
}
