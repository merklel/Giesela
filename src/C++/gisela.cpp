#include "gisela.hpp"

#include <utility>



//-------------------- Klasse Tank--------------------------------------------
//----------------------------------------------------------------------------
// Constructor
Tank::Tank(float vol_max, float vol_is) {
    volume_max = vol_max;
    volume_is = vol_is;
}

void Tank::setVolume(float vol) {
    volume_is = vol;
}

void Tank::resetVolume() {
    volume_is = volume_max;
}

void Tank::decrementVolume(float dvol) {
    volume_is = volume_is - dvol;
}

float Tank::getVolume() {
    return volume_is;
}
//----------------------------------------------------------------------------






//--------------------klasse gisela-------------------------------------------
//----------------------------------------------------------------------------
// Constructor
Gisela::Gisela() {
    giessenActive = 0;
    tank        = new Tank(TANKVOLUME, TANKVOLUME);
    hardware    = new Hardware();
    unixSocket1 = new UnixSocket(SOCKET_PATH);
    restApi     = new RestApi();
    restApi->setGisela(this);

    clock_gettime(CLOCK_REALTIME, &startTime);
    std::cout << ctime(&startTime.tv_sec) << std::endl;

    //setup started message
    std::string mes = "--------Gisela startet at ";
    mes.append(ctime(&startTime.tv_sec));
    this->writeLog(mes);

    //Populate config
    config["lastWater"] = "Mon Feb  8 01:40:31 1970";
    config["waterReset"] = "0";
    config["time1"] = "0";
    config["time2"] = "0";
    config["time3"] = "0";
    config["level"] = "0";

    lastGiessTime = 0;

    giessdauer_s = 10*60;

    flag_stop_giessen = 0;
    flag_stop_giessen = 0;

    trustyMailAdresses.push_back("evi.wiedemann@googlemail.com");
    trustyMailAdresses.push_back("lukas.merkle@live.de");

    trigger_regel = 0;


}

// Destructor
Gisela::~Gisela() {
    delete tank;
    delete hardware;
    delete unixSocket1;
}

int Gisela::getGiessStatus() {
    return giessenActive;
}

time_t *Gisela::getStartTime() {
    return &(startTime.tv_sec);
}

time_t *Gisela::getLastGiessZeit() {
    return &lastGiessTime;
}

void Gisela::writeConfig() {
    std::ofstream file;

    //Update config FElder
    //config["time3"] = "2";
    config["level"] = std::to_string(this->tank->getVolume());
    std::string lgz = ctime(this->getLastGiessZeit());
    config["lastWater"] = lgz.substr(0, lgz.size()-1);

    file.open(CONFIG_FILE);
    for (auto it = config.cbegin(); it != config.cend(); ++it) {
        file << (*it).first << " " << (*it).second << std::endl;
    }
    //file << giessenActive << std::endl << ctime(&lastGiessTime) << std::endl;
    file.close();
}

void Gisela::readConfig() {
    std::cout << "reading Config..." << std::endl;
    std::ifstream file;
    file.open(CONFIG_FILE, std::ifstream::in);


    //Read in lines of config file until eof()
    while (!file.eof()) {
        //read line
        char input[200];
        file.getline(input, 200);
        std::string input_str{input};
        //std::cout << input_str << std::endl;
        std::size_t posSpace = input_str.find(" ");

        std::map<std::string, std::string>::iterator it = config.find(input_str.substr(0, posSpace));
        if (it != config.end()) {
            config[input_str.substr(0, posSpace)] = input_str.substr(posSpace + 1, input_str.length());
        }
    }
    this->tank->setVolume(std::stof(config["level"]));
    file.close();
}

json Gisela::readJsonConfigFromDisk(){
    std::ifstream f(CONFIG_JSON);
    json data = json::parse(f);
    this->config2 = data;
    return data;
}

void Gisela::writeJsonConfigToDisk() {
    std::ofstream o(CONFIG_JSON);
    o << std::setw(4) << this->config2 << std::endl;
}

json Gisela::getJsonConfig(){
    // returns the current state of the config from inside Gisela class.
    // Does not re-read from disk
    return this->config2;
}


void Gisela::set_current_config_via_api(bool b_slot1, bool b_slot2, bool b_slot3, std::string t_slot1, std::string t_slot2, std::string t_slot3, int dur_slot1, int dur_slot2, int dur_slot3){
/*
 * This function gets called from the api and alters the internal state of Gisela. This does not
 * persist any config to file.
 */
    this->config2["t_slot1"] = t_slot1;
    this->config2["t_slot2"] = t_slot2;
    this->config2["t_slot3"] = t_slot3;

    this->config2["b_slot1"] = b_slot1;
    this->config2["b_slot2"] = b_slot2;
    this->config2["b_slot3"] = b_slot3;

    this->config2["duration_slot1"] = dur_slot1;
    this->config2["duration_slot2"] = dur_slot2;
    this->config2["duration_slot3"] = dur_slot3;

}

void Gisela::writeLog(std::string message) {
    //Generate timestamp for logfile
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    std::string timestamp{ctime(&t.tv_sec)};

    timestamp.resize(24);

    std::ofstream file;
    file.open(LOG_FILE, std::ios::app);
    timestamp.append(" ");
    timestamp.append(message);
    file << timestamp << std::endl;
    file.close();
}

std::map<std::string, std::string> Gisela::getConfig() {
    //Print out config map
    std::cout << "config map contains:" << std::endl;
    for (auto it = config.cbegin(); it != config.cend(); ++it) {
        std::cout << " [" << (*it).first << ':' << (*it).second << ']';
        std::cout << '\n';
    }

    return config;
}

time_t maketime(std::string input){
    struct tm tm;
    strptime(input.c_str(), "%H:%M", &tm);
    time_t t = mktime(&tm);
    return t;
}

std::vector<time_t> Gisela::get_times(){
    std::vector<time_t> times;
    times.push_back(maketime(this->config2["t_slot1"]));
    times.push_back(maketime(this->config2["t_slot2"]));
    times.push_back(maketime(this->config2["t_slot3"]));
    std::cout << "Inside gettimes: " << times[1] << std::endl;
    return times;
}

std::vector<bool> Gisela::get_b_slots(){
    std::vector<bool> b_slots;
    b_slots.push_back(this->config2["b_slot1"]);
    b_slots.push_back(this->config2["b_slot2"]);
    b_slots.push_back(this->config2["b_slot3"]);
    return b_slots;
}

std::vector<int> Gisela::get_dur_slots(){
    std::vector<int> d_slots;
    d_slots.push_back(this->config2["duration_slot1"]);
    d_slots.push_back(this->config2["duration_slot2"]);
    d_slots.push_back(this->config2["duration_slot3"]);
    return d_slots;
}

void Gisela::funktionGiessen(int seconds) {


    //Checking the Argument: nolonger pumping then 20 min! _--!!
    //----------------------------------------------------------------
    if (seconds < 60 * 20) {

        // Timestamp fuer config. lastGiessTime ist Klassenvariable 
        struct timespec t;
        clock_gettime(CLOCK_REALTIME, &t);
        lastGiessTime = t.tv_sec;

        std::chrono::steady_clock::time_point t_start = std::chrono::steady_clock::now();
        giessenActive = 1;
        //#--- 1 ------
        // ---------------- einschalten von PumpenPins etc. -----------------------------//
        this->hardware->pumpe->switch_on();
        this->hardware->led_rot->switch_on();


        //#--- 2 ------
        // ------------ Keep Gisela running for int seconds------------------------------//
        for (int i = 0; i < seconds; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            this->tank->decrementVolume(LPERS);
            std::cout << "Tankinhalt: " << this->tank->getVolume() << "\r";
            std::cout << std::flush;

            // If flag_stop_giessen set to 1 -> break out of for loop and do #3
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
        this->hardware->led_rot->switch_off();
        giessenActive = 0;
        std::cout << std::endl << "Giessen beendet..." << std::endl;
        this->writeLog("giessen beendet...");
        // Trigger regel wieder auf 0, damit wieder freigeschaltet.
        trigger_regel = 0;
       
        //save config lastWater
        this->writeConfig();


    //--------- if argument > 20 min --> direct abort
    //--------------------------------------------------
    }else{
        std::cout << "kann nicht mehr als 20 min auf einmal giessen!" << std::endl;
        this->writeLog("Giessen fehlerhaft beendet, Argument seconds zu hoch (>20min)...");
    }

}

void Gisela::startGiessen(int giessdauer_s_arg) {
    int dauer_giessen_sekunden = 0;
    if (giessdauer_s_arg <= 0){
        dauer_giessen_sekunden = giessdauer_s; // Interne Fallbackebene, giessdauer in klasse definiert
    }else{
        dauer_giessen_sekunden = giessdauer_s_arg; // Nehme argument als giessdauer, wenn größer null
    }

    this->readConfig();
    //std::map<std::string, std::string> config = this->getConfig();
    std::cout << "letztes mal giessen: " << config.at("lastWater") << std::endl;

    // Logik last water hier!
    std::string lw = config.at("lastWater");
    // Hole aktuelle zeit
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);
    int hour 	= aTime->tm_hour;
    int minute 	= aTime->tm_min;
    int day 	= aTime->tm_yday;	

    // finde tag in lw aus config
    size_t pos = lw.find(":");
    int lw_day = std::stoi(lw.substr(pos-5,2));
    // finde uhrzeit in lw aus config
    pos = lw.find(":");
    int lw_hour = std::stoi(lw.substr(pos-2,2));
    int lw_minute = std::stoi(lw.substr(pos+1,2));

    if ((std::abs(lw_hour - hour) >= 2) || (lw_day != day)){ // Max X mal pro stunde, oder an einem anderen Tag

        if (giessenActive == 0){

            this->writeLog("giessen startet...");
            std::string message;
            message = std::string("giessen fuer: ") + std::to_string(dauer_giessen_sekunden) + std::string(" Sekunden.");
            this->writeLog(message);
            std::cout << "giessen startet..." << std::endl;

            std::thread giessen(&Gisela::funktionGiessen, this, dauer_giessen_sekunden);
            giessen.detach();
        }else{
            std::cout << "Giessen schon aktiv! Kann giessen nicht starten" << std::endl;
            this->writeLog("giessen abgebrochen, schon aktiv...");
        }
    }else{
        std::cout << "Letztes Giessen ist noch nicht lange genug her!" << std::endl;
        this->writeLog("giessen abgebrochen, noch nicht lange genug her...");
    }
}

void Gisela::stopGiessen() {
    flag_stop_giessen = 1;
}


[[noreturn]] int Gisela::operating(){
    // Funktion beinhaltet den Main-while(1) Loop
    while(true){
        usleep(5000);
         //std::cout << "..." << std::endl;
        //this->hardware->led_gruen->switch_on();
        //this->hardware->led_rot->switch_off();

        //std::this_thread::sleep_for(std::chrono::seconds(1));
        //this->hardware->led_gruen->switch_off();
        //this->hardware->led_rot->switch_on();

        //std::this_thread::sleep_for(std::chrono::seconds(1));

        //std::cout << this->hardware->button_gruen->get_status() << std::endl;



        // Read Data from Socket.
        std::string socketdata = this->unixSocket1->getSocketBuffer();
        if (!socketdata.empty()){
            std::cout << "Input from Socket: "<< socketdata << std::endl;
        }

        
        // Poll Buttons
        int button_rot_status = this->hardware->button_rot->get_status();
        int button_gruen_status = this->hardware->button_gruen->get_status();

        if ( button_rot_status== 1){
            std::cout << "Roter Button gedrueckt!"<< socketdata << std::endl;
        }else if (button_rot_status == 2){
            std::cout << "Roter Button lange gedrueckt!"<< socketdata << std::endl;
        }

        if (button_gruen_status == 1){
            std::cout << "Gruener Button gedrueckt!"<< socketdata << std::endl;
        }else if (button_gruen_status == 2){
            std::cout << "Gruener Button lange gedrueckt!"<< socketdata << std::endl;
        }


        // Auf Socket == 2 reagieren und Config neu einlesen!
        if (socketdata.compare("2") == 0){
            socketdata.clear();
            this->readConfig();
            this->getConfig();
            if (config["waterReset"] == "1"){
                config["waterReset"] = "0";
                std::cout << "Wassertank auf 30l resettet! " << std::endl;
                this->tank->setVolume(TANKVOLUME);
                this->writeConfig();
            }
            this->writeLog("Socket==2: Neue Config eingelesen.");
        }


        // Gießbedingungen checken 
        // 1. Wenn socket == 1
        if (socketdata.compare("1") == 0){
            socketdata.clear();
            this->startGiessen(0);
            this->writeLog("Socket==1: Giessen getriggert.");
            usleep(1000000); // 1s warten zur entprellung des threads
        }

        // 2. Wenn gruener Button kurz gedrueckt wird, giessen
        if (button_gruen_status == 1){
            this->startGiessen(0);
            this->writeLog("Grüner Button == 1: Giessen getriggert.");
            usleep(1000000); // 1s warten zur entprellung des threads
        }

        // 3. Wenn roter Button kurz gedrueckt wird: giessen stoppen
        if (button_rot_status == 1){
            this->stopGiessen();
            this->writeLog("Roter Button: Giessen wird gestoppt.");
        }

        // 4. Checke Mail-Interface
        int mi = this->getTriggerMailStatus();

        if (mi == 1){
            this->startGiessen(0);
            this->resetTriggerMailStatus();
            this->writeLog("gültige Mail erhalten: Giessen wird getriggert.");
            usleep(1000000); // 1s warten zur entprellung des threads
        }

        // 5. Regelmaessiges giessen checken

        // Hole aktuelle zeit
        time_t theTime = time(NULL);
        struct tm *aTime = localtime(&theTime);
        int hour=aTime->tm_hour;
        int minute=aTime->tm_min;

        int gies_t_min=0;

        // --- Deprecated: Aus alter config ---------------------------------------------------
        // Hole die drei configs aus dem config map
        /*
        std::vector<std::string> slots;
        slots.push_back(config["time1"]);
        slots.push_back(config["time2"]);
        slots.push_back(config["time3"]);

        // Stunden und Minuten aus dem config-map herausfiltern
        std::vector<int> config_hours;
        std::vector<int> config_minutes;
        std::vector<int> config_gimes;

        for(int i=0; i < 3; ++i){
            std::size_t pos = slots.at(i).find(":");
            config_hours.push_back(std::stoi(slots.at(i).substr(2, pos)));
            config_minutes.push_back( std::stoi( slots.at(i).substr(pos+1, 2) ));
            std::size_t pos_gimes_semicolons = slots.at(i).find(";");
            config_gimes.push_back( std::stoi(slots.at(i).substr(pos_gimes_semicolons+1)) );
        }

        //std::cout << hour << ":"<< minute<< std::endl;
        for(int i=0; i < 3; ++i){
            if (slots.at(i).substr(0,1) == "1"){
                if ((config_hours.at(i) == hour) && (config_minutes.at(i) == minute)){
                    if (trigger_regel == 0){
                        std::cout << "Slot " << i << " aktiv!" << std::endl;
                        trigger_regel = 1;
                        gies_t_min = config_gimes.at(i);
                    }
                }
            }
        }
         */

        // --- New: Aus json config ---------------------------------------------------
        // automatisches giessen aus config2 lesen

        this->readJsonConfigFromDisk();
        std::vector<time_t> times = this->get_times();
        std::vector<bool> b_slots = this->get_b_slots();
        std::vector<int> dur_slots = this->get_dur_slots();

        for(int i=0; i < 3; ++i){
            if (i==1){
                std::cout << "i: " << i << "------------------------" << std::endl;
                std::cout << "set H: " << gmtime(&times[i])->tm_hour << ", " << times[i] <<  " Ist: " << hour << " Config: " << this->config2["t_slot1"] << " raw times[i]: " << times[i] << std::endl;
                std::cout << "set M: " << gmtime(&times[i])->tm_min << " Ist: " << minute << std::endl;
                std::cout << "set B: " << b_slots[i] << std::endl;
                std::cout << "--------------------------------------" << std::endl;
            }
            if (gmtime(&times[i])->tm_hour == hour && gmtime(&times[i])->tm_min == minute && b_slots[i]){
                if (trigger_regel == 0){
                    std::cout << "From new config: Slot " << i << " aktiv!" << std::endl;
                    trigger_regel = 1;
                    gies_t_min = dur_slots[i];
                }
            }
        }

        if (trigger_regel == 1 && giessenActive == 0){
            this->writeLog("Regel-Giessen: Giessen wird getriggert.");
            this->startGiessen(gies_t_min * 60);
            usleep(2000000); // 2s warten zur entprellung des threads
        }
    }
}



