#include "hardware.hpp"
#include <pigpio.h>
#include <iostream>


//--------------- Definition of ButtonCode -----------------
//Constructor 
Button::Button(int hwpin){
	pin = hwpin;
}

int Button::get_status(){
    status = gpioRead(pin);

	if (status != 0){
		usleep(30000); // 0,03s warten zum entprellen

		if (gpioRead(pin) != 0){
			usleep(600000); // 0.6 sekunden warten

			if (gpioRead(pin) != 0){
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
    gpioWrite(pin, 1);
	status = 1;
}

void Led::switch_off(){
    gpioWrite(pin, 0);
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
    gpioWrite(pin, 1);
	status = 1;
}

void Pumpe::switch_off(){
    gpioWrite(pin, 0);
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

    if (gpioInitialise() < 0)
    {
        std::cout << "Initialized pigpio failed!" << std::endl;
    }
    else
    {
        std::cout << "Initialized pigpio: ok!" << std::endl;
    }

    gpioSetMode(PIN_PUMPE, PI_OUTPUT);
    gpioSetMode(PIN_LED_GRUEN, PI_OUTPUT);
    gpioSetMode(PIN_LED_ROT, PI_OUTPUT);
    gpioSetMode(PIN_TASTER_GRUEN, PI_INPUT);
    gpioSetMode(PIN_TASTER_ROT, PI_INPUT);

    gpioWrite(PIN_PUMPE, 0);
    gpioWrite(PIN_LED_GRUEN, 0);
    gpioWrite(PIN_LED_ROT, 0);

    return 0;
}
//----------------------------------------------------------