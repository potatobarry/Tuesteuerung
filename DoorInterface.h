/*  -*- mode: c++; -*-

 * Praktikum MRT2 
 * ART1 Türsteuerung
 * Institut für Automatisierungstechnik
 * letztes Update Jun 2021
 * Author: M. Herhold
 * Version: r5
 */

#ifndef DOOR_INTERFACE_HH
#define DOOR_INTERFACE_HH

#include <thread>
#include <string>
#include <usb.h>
#include <atomic>


/* 
 * DoorInterface deklariert Funktionen und atomare Variablen zur Benutzung des E/A Geraetes. 
 * TODO aufteilen auf mehrere Klassen um Oberfläche, Simulator und Hardware-Interface klar zu trennen
 * TODO Interrupt-Timer implementieren
 */
class DoorInterface{
    public:
	/* 
	 * Boolsche Variable zum Detektieren des gewuenschten Programmabbruchs
	 * `true` bedeutet der Programmabbruch ist gewuenscht
	 * den Variablenwert regelmaessig pruefen!
	 */
	static std::atomic<bool> quit_doorcontrol_flag;

	static DoorInterface& get_instance() {
		static DoorInterface _instance;
		return _instance;
	}

	// Wissenswertes über statische Member und Methoden:
	// https://www.learncpp.com/cpp-tutorial/static-member-functions/


	
	/* Destruktor */
	~DoorInterface(void);

	/*
	 * Liest alle Pins eines Eingabe-Ports.
	 * port: Port-Nummer
	 * bits: ausgelesenen Kanäle des Ports (Rückgabewert)
	 * weitere Informationen, siehe Praktikumsanleitung
	 */
	void DIO_Read(const unsigned port, unsigned char *pins);
	

	/*
	 * Ändert alle Pins eines Ausgabe-Ports.
	 * port: Ausgabe-Port-Nummer
	 * pins: die auszugebenden Kanäle des Ports
	 */
	void DIO_Write(const unsigned port, const unsigned char pins);	

	/* 
	 * Methode zum Anzeigen eines Strings in der Nachrichtenzeile eines Displays
	 * s: auszugebender String (maximale Länge 50 alphanumerische Zeichen) 
	 * set debug string for showing in Simulation UI */
	void DebugString(const std::string s);

    private:
		/*
		 * Konstruktor des Interface zur Hardware / Simulation
		 * real_door: legt fest ob auf die Hardware zugegriffen werden soll
		 * show_ui:   legt fest, ob die Ein-/Ausgabe ueber ncurses genutzt wird
		 * wirft Exception `std::runtime_error` im Fehlerfall
		 *
		 * wenn "show_ui" wahr ist, dann nutze zum Starten des Programms in Eclipse:
		 * "External Tools" -> run in xterm"
		 */
		DoorInterface(bool real_door = false, bool show_ui = true);
		DoorInterface(const DoorInterface&);
		DoorInterface& operator = (const DoorInterface&);

		const bool real_door;
		const bool show_ui;
	
		std::thread ui_thread;
		char debug_string[100];
	
		struct usb_device *dev;
		struct usb_dev_handle *handle;
	
		int channels;
		int sim_channels;
	
		void HandleSimUI(void);
		void InitNcurses(void);
		void ShowSimUI(void);
		void HandleSimInput(int c);
};

#endif // DOOR_INTERFACE_HH
