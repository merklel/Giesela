#ifndef HARDWARE_H
#define HARDWARE_H

#include <unistd.h>


// Pindefinitionen Wiring pi Pin nummerierung
#define PIN_PUMPE 15      // pigpio 14      wiring pi 15
#define PIN_LED_GRUEN 21  // pigpio 5       wiring pi 21
#define PIN_LED_ROT 22    // pigpio 6       wiring pi 22
#define PIN_TASTER_GRUEN 5// pigpio 24      wiring pi
#define PIN_TASTER_ROT 4  // pigpio 23



class Button{
private:
	int status;
	int pin;
public:
	//Constructor
	Button(int hwpin);

	int get_status();

};

class Led{
private:

public:
	int status;
	int pin;
	//Constructor
	Led(int hwpin);

	int get_status();
	int get_pin();
	void switch_on();
	void switch_off();

};

class Pumpe{
private:
	int status;
	int pin;
public:
	//Constructor
	Pumpe(int hwpin);

	int get_status();
	void switch_on();
	void switch_off();

};



class Hardware{

private:


public:
	Led * led_gruen;
	Led * led_rot;
	Pumpe * pumpe;
	Button * button_gruen;
	Button * button_rot;

	//Constructor
	Hardware();

	int initGpio();



};


















#endif