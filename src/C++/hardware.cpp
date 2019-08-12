#include "hardware.hpp"
#include <wiringPi.h>


//--------------- Definition of ButtonCode -----------------
//Constructor 
Button::Button(int hwpin){
	pin = hwpin;
}

int Button::get_status(){
	status = digitalRead(pin);

	if (status != 0){
		usleep(30000); // 0,03s warten zum entprellen

		if (digitalRead(pin) != 0){
			usleep(600000); // 0.6 sekunden warten

			if (digitalRead(pin) != 0){
				status = 2;
			}else{
				status = 1;
			}	
		}
	}

	return status;
}
//----------------------------------------------------------







//---------------- Definition LED Code ---------------------
// Constructor
Led::Led(int hwpin){
	pin = hwpin;
}

int Led::get_status(){
	return status;
}

int Led::get_pin(){
	return pin;
}

void Led::switch_on(){
	digitalWrite(pin, 1);
	status = 1;
}

void Led::switch_off(){
	digitalWrite(pin, 0);
	status = 0;
}
//----------------------------------------------------------








//---------------- Definition Pumpe Code -------------------
// Constructor
Pumpe::Pumpe(int hwpin){
	pin = hwpin;
}

int Pumpe::get_status(){
	return status;
}

void Pumpe::switch_on(){
	digitalWrite(pin, 1);
	status = 1;
}

void Pumpe::switch_off(){
	digitalWrite(pin, 0);
	status = 0;
}

//----------------------------------------------------------







//---------------- Definition Hardware Code ----------------
//Constructor
Hardware::Hardware(){
	led_gruen 		= new Led(PIN_LED_GRUEN);
	led_rot   		= new Led(PIN_LED_ROT);
	pumpe     		= new Pumpe(PIN_PUMPE);
	button_gruen 	= new Button(PIN_TASTER_GRUEN);
	button_rot   	= new Button(PIN_TASTER_ROT);
}

int Hardware::initGpio() {

    wiringPiSetup();

    pinMode(PIN_PUMPE, OUTPUT);
    pinMode(PIN_LED_GRUEN, OUTPUT);
    pinMode(PIN_LED_ROT, OUTPUT);
    pinMode(PIN_TASTER_GRUEN, INPUT);
    pinMode(PIN_TASTER_ROT, INPUT);

    digitalWrite(PIN_PUMPE, 0);
    digitalWrite(PIN_LED_GRUEN, 0);
    digitalWrite(PIN_LED_ROT, 0);

    return 0;
}
//----------------------------------------------------------