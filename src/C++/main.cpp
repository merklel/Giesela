

#include "gisela.hpp"
#include "UnixSocket.h"
// username: ***
// passwd raspberry: ***** 


int main(){

	// ----------------- Initialisierungen -----------------------------------------------

	//instantiate gisela object
	Gisela gisela;

	//std::cout << "Wasserstand: " << gisela.tank->getVolume() << std::endl;
	//std::cout << "Status: " << gisela.getGiessStatus() << " letzes Giessen: " << ctime(gisela.getLastGiessZeit()) << std::endl;

	std::cout << "initializing and reading config file..."<< std::endl;
	gisela.readConfig();
   	gisela.getConfig();
	//gisela.writeConfig();


    std::cout << "starting mail server..."<< std::endl;
    gisela.startMailServer();


    std::cout << "starting socket server..."<< std::endl;
    gisela.unixSocket1->startSocketServer();


    std::cout << "initializing hardware..."<< std::endl;
    gisela.hardware->initGpio();

    // -----------------------------------------------------------------------------------



    // --------------- Starting Main Loop inside operating() -----------------------------
    gisela.operating();
    // -----------------------------------------------------------------------------------


	return 0;
}