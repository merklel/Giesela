#ifndef GISELA_H
#define GISELA_H

#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fstream>
#include <ostream>
#include <string>
#include <map>
#include <curl/curl.h>
#include <regex>
#include <vector>
#include <thread>
#include <chrono>
#include "UnixSocket.h"
#include "hardware.hpp"
#include "UnixSocket.h"
#include "restapi.hpp"
#define CONFIG_FILE "/home/pi/Giesela/gSockets/config.txt"
#define CONFIG_JSON "/home/pi/Giesela/var/conf/config.json"
#define LOG_FILE "/home/pi/Giesela/var/log/log.txt"
#define SOCKET_PATH "/home/pi/Giesela/gSockets/gSocketCpp.sock"
#define MAIL_CREDS_FILE "/home/pi/Giesela/var/security/mailCreds.json"
#define LPERM 0.34 //Evtl anpassen auf 0.36 nach 1 Tag test
#define LPERS (0.34/60)
#define TANKVOLUME 30
#include "nlohmann/json.hpp"



using json = nlohmann::json;


class Tank{
private:
	float volume_max;
	float volume_is;
public:
	//Konstruktor
	Tank(float vol_max, float vol_is);
	void setVolume(float vol);
	void resetVolume();
	void decrementVolume(float dvol);
	float getVolume();
};


class Gisela{
private:
		int giessenActive;
		struct timespec startTime;
		time_t lastGiessTime;
        std::vector<int> unseenMailId;
        int giessdauer_s;
		int flag_stop_giessen;

		int got_trigger_mail;
		std::vector<std::string> trustyMailAdresses;

		int trigger_regel;

public:
	Tank * tank;
	UnixSocket * unixSocket1;
	Hardware * hardware;
	RestApi * restApi;
	std::map <std::string, std::string> config;
	json mailCreds;
	json config2;

	//Konstruktor
	Gisela();
	//Destruktor
	~Gisela();

    

	int getGiessStatus();
	time_t * getStartTime();
	time_t * getLastGiessZeit();
	std::map <std::string, std::string> getConfig();
	void writeConfig();
	void readConfig();
	json readJsonConfigFromDisk();
	void writeJsonConfigToDisk();
	json getJsonConfig();
	void writeLog(std::string message);
	void set_current_config_via_api(bool b_slot1, bool b_slot2, bool b_slot3, std::string t_slot1, std::string t_slot2, std::string t_slot3, int dur_slot1, int dur_slot2, int dur_slot3);

	std::string getLog();

	// Funktionen definiert in Mailinterface.cpp
	int checkForNewMails();
    int readMails();

    std::vector<tm> get_times();
    std::vector<bool> get_b_slots();
    std::vector<int> get_dur_slots();

    [[noreturn]] void MailServer_thread();
    void startMailServer();
    int getTriggerMailStatus();
    void resetTriggerMailStatus();
    void readMailCreds();


    void funktionGiessen(int seconds);
    void startGiessen(int giessdauer_s_arg);
	void stopGiessen();

    [[noreturn]] int operating();
};












#endif
