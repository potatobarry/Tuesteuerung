/*
 * Praktikum MRT2 
 * ART1 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Jun 2021
 * Co-Autor: M. Herhold
 * Version: r5
 */

#include "DoorInterface.h"
#include "Library/niusb6501.h"
#include <ncurses.h>
#include <iostream>
#include <string.h>
#include <fstream>

// failsave if exit() function gets called by class user
// it is not encouraged to use exit()
static void end_ncurses(void)
{
	endwin();
}

//static member for exit requests
std::atomic<bool> DoorInterface::quit_doorcontrol_flag(false);

DoorInterface::DoorInterface(bool real_door, bool show_ui) : real_door(real_door),
							     show_ui(show_ui),
							     ui_thread(),
							     channels(0xFFFF),
							     sim_channels(0)
{
	if(show_ui){
		InitNcurses();
		ui_thread = std::thread(&DoorInterface::HandleSimUI, this);
	}	

	if(!real_door)
		return;
	
	if( niusb6501_list_devices(&dev, 1) != 1) {
		throw std::runtime_error("Device not found");
	}
	handle = niusb6501_open_device(dev);
	if (handle == NULL){
		throw std::runtime_error("Unable to open the USB device");
	}
	niusb6501_set_io_mode(handle, 0x03, 0x00, 0xFF);
}

DoorInterface::~DoorInterface(void)
{
	if(show_ui){
		quit_doorcontrol_flag=true;
		if(ui_thread.joinable()) ui_thread.join();
		endwin();
	}
}

void DoorInterface::HandleSimUI(void){
	while(!quit_doorcontrol_flag){
		HandleSimInput(getch());
		ShowSimUI();
		std::this_thread::sleep_for( std::chrono::milliseconds(40) );
	}
}

void DoorInterface::InitNcurses(void)
{
	atexit(end_ncurses);
	initscr();		// initialisiere die curses Bibliothek
	keypad(stdscr, TRUE);	// schalte das Tastatur-Mapping ein
	raw();			// take input chars one at a time, no wait for \n
	curs_set(0);		// Cursor ausschalten
	noecho();		// echo input
	nodelay(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_RED, COLOR_WHITE);
	bkgd(COLOR_PAIR(3));

	DebugString("unset");
}

void DoorInterface::DebugString(const std::string s)
{
	//sprintf(debug_string, "%.50s", s.c_str());
	int i=0, j=0;
	const char* tmp = s.c_str();
	int j_max = s.size();
	
	while(i < 50){ //needed to clean ncurses
		if( (j < j_max) && (isalnum( tmp[j] )) ){
			debug_string[i] = tmp[j];
		} else {
			debug_string[i] = ' ';
		}
		i++;
		j++;
	}
	debug_string[i]='\0';

	std::ofstream logfile;
	logfile.open ("interface.log", std::ios::out | std::ios::app);
	logfile << s << std::endl;		//writes s into new line
	logfile.close();

}


// zeigt die Simulatoroberfläche
void DoorInterface::ShowSimUI(void)
{
	//Anzeige des Betriebsart
	color_set(4, 0);
	if( ((channels >> 0) & 1)) {
		if( ((channels >> 1) & 1))
			mvprintw(1, 30, "Automatikbetrieb    ");
		else
			mvprintw(1, 30,"Handbetrieb         ");
	} else {
		if( ((channels >> 1) & 1))
			mvprintw(1, 30, "Reparaturbetrieb    ");
		else
			mvprintw(1, 30, "Prozesssteuerung aus");
	}
	
	color_set(3, 0);
	attron(A_BOLD);
	color_set(3, 0);
	mvprintw(5, 9, "Eingänge");
	mvprintw(5, 53, "Ausgänge");
	attroff(A_BOLD);

	//Eingänge
	color_set(1, 0);
	mvprintw(7, 9, "BW1 BW2 NTA NTZ ELO  X  ELG LSH LSV  BM  X  ");	
	move(8, 9);
	for (int i=0; i<11; i++)
		printw(" %ld  ", (channels>>i)&1);
	printw("");

	//Ausgänge
	color_set(2, 0);			
	mvprintw(7, 55, " Y1 Y2 Y3  Bedeutung ");
	mvprintw(8, 55, "");
	for (int i=0; i<3; i++)
		printw(" %ld ", (sim_channels>>i)&1);
	printw("  ");

	//Bedeutung der Ausgänge
	{
		bool LL = ((sim_channels >> 0) & 1);
		bool RL = ((sim_channels >> 1) & 1);
		bool WL = ((sim_channels >> 2) & 1);

		static unsigned blink = 0;
		blink++;
		if(WL && (blink & 8)){
			mvprintw(8, 72, "(o) ");
		} else {
			mvprintw(8, 72, "    ");
		}
		if(LL && RL){
			mvprintw(8, 66, " ERR  ");
		} else if(!LL && !RL){
			mvprintw(8, 66, " stop ");
		} else if(LL) {
			mvprintw(8, 66, " open ");
		} else {
			mvprintw(8, 66, "close ");
		}
	}
	
	color_set(3,0);
	//Tastenbelegung
	mvprintw(9, 2, " Shift+ F1  F2  F3  F4  F5  F6  F7  F8  F9  F10 F11");

	//Erklärung
	attron(A_BOLD);

	mvprintw(11, 15, "Kürzel - Taste : Bedeutung");
	attroff(A_BOLD);
	mvprintw(12, 15, "  BW1  -  F1   : Betriebswahlschalter 1");
	mvprintw(13, 15, "  BW2  -  F2   : Betriebswahlschalter 2");
	mvprintw(14, 15, "  NTA  -  F3   : Nutzer-Taster öffnen");
	mvprintw(15, 15, "  NTZ  -  F4   : Nutzer-Taster schliessen");
	mvprintw(16, 15, "  ELO  -  F5   : Tür vollständig offen");
	mvprintw(17, 15, "  X    -  F6   : Reserviert");
	mvprintw(18, 15, "  ELG  -  F7   : Tür vollständig geschlossen");
	mvprintw(19, 15, "  LSH  -  F8   : Lichtschranke horizontal");
	mvprintw(20, 15, "  LSV  -  F9   : Lichtschranke vertikal");
	mvprintw(21, 15, "  BM   -  F10  : Bewegungsmelder");
	mvprintw(22, 15, "  X    -  F11  : Reserviert");

	mvprintw(24, 14, "Message : %.50s", debug_string);
	
	move(2, 14);
	refresh();

}

void DoorInterface::HandleSimInput(int c)
{
	if (c == 'q') { 	//'q' = 113
		// quit request via keyboard
		quit_doorcontrol_flag=true;		
	}
	
	if(real_door)
		return;

	#ifdef DEBUG_NCURSESUI
	mvprintw(15, 10, "%i ", c); //Debug
	#endif
	
	int chan, cbit;
	// Switch channels
	// SHIFT+{F1-F12}
	if( (c >= 277) && (c <= 288) ){
		chan = c - 277;
		cbit = (channels >> chan) & 1; // ist der Channel z.Zt. gesetzt?
		if (cbit)
			channels = channels ^ (0x1L << chan);
		else
			channels = channels | (0x1L << chan);
	}

	#ifdef DEBUG_NCURSESUI
	switch(c){
	case 56: sim_channels = sim_channels ^ (1 << 0);
		break;
	case 57: sim_channels = sim_channels ^ (1 << 1);
		break;
	case 48: sim_channels = sim_channels ^ (1 << 2);
		break;
	default: break;
	}	
	#endif

}

void DoorInterface::DIO_Read(const unsigned port, unsigned char *pins)
{
	if( !real_door ){
		switch(port){
		case 0:
			*pins = (unsigned char)(channels & 0xFF);
			break;
		case 1:
			*pins = (unsigned char)((channels >> 8) & 0xFF);
			break;
		default:
			*pins = (unsigned char)(rand() & 0xFF);			
		}
		return;
	}
	
	int status;
	
	status = niusb6501_read_port(handle, port, pins);
	if(status) {
		std::cerr << "error read port " << port << ": " << strerror(-status) << std::endl;
	}
}

void DoorInterface::DIO_Write(const unsigned port, const unsigned char pins)
{
	if(port != 2){
		std::cerr << "error write to port " << port << std::endl;
		return;
	}
	
	sim_channels = pins;

	if( !real_door ){
		return;
	}
	
	unsigned char p;
	int status;
	
	p = pins ^ 0xFF;
	status = niusb6501_write_port(handle, 2, p);
	if(status) {
		std::cerr << "error write port 2: " << strerror(-status) <<std::endl;
	}
}
