#include "gisela.hpp"


void Gisela::funktionGiessen(int seconds) {


    //Checking the Argument: nolonger pumping then 20 min! _--!!
    //----------------------------------------------------------------
    if (seconds < 60 * 20) {

    	// Timestamp fuer config. lastGiessTime ist Klassenvariable 
    	struct timespec t;
    	clock_gettime(CLOCK_REALTIME, &t);
    	lastGiessTime = t.tv_sec;

        std::cout << "thread start! " << "\n";
        std::chrono::steady_clock::time_point t_start = std::chrono::steady_clock::now();
        giessenActive = 1;
        //#--- 1 ------
        // ---------------- einschalten von PumpenPins etc. -----------------------------//
        this->hardware->pumpe->switch_on();


        //#--- 2 ------
        // ------------ Keep Gisela running for int seconds------------------------------//
        for (int i = 0; i < seconds; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            this->tank->decrementVolume(LPERS);
            std::cout << "Tankinhalt: " << this->tank->getVolume() << "\r";
            std::cout << std::flush;

            // If flag_stop_giessen set to 1 -> break out of for loop and do #
            if (flag_stop_giessen == 1){
                std::cout << std::endl << "Giessen abgebrochen..." << std::endl;
                this->writeLog("giessen abgebrochen...");
                flag_stop_giessen = 0;
                break;
            }
        }

        //#--- 3 ------
        // ---------------- abschalten von PumpenPins etc. -----------------------------//
        this->hardware->pumpe->switch_off();
        giessenActive = 0;
        std::cout << std::endl << "Giessen beendet..." << std::endl;
        this->writeLog("giessen beendet...");
       
        //save config lastWater
    	this->writeConfig();


    //--------- if argument > 20 min --> direct abort
    //--------------------------------------------------
    }else{
        std::cout << "kann nicht mehr als 20 min auf einmal giessen!" << std::endl;
        this->writeLog("Giessen fehlerhaft beendet, Argument seconds zu hoch (>20min)...");
    }

}

void Gisela::startGiessen() {

    this->writeLog("giessen startet...");
    std::cout << "giessen startet..." << std::endl;

    std::thread giessen(&Gisela::funktionGiessen, this, giessdauer_s);
    giessen.detach();


}

void Gisela::stopGiessen() {

    flag_stop_giessen = 1;
}

