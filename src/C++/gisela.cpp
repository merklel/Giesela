#include "gisela.hpp"



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


int Gisela::operating(){
    // Funktion beinhaltet den Main-while(1) Loop
    while(1){
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
        // Hole die drei configs aus dem config map
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

        // Hole aktuelle zeit
        time_t theTime = time(NULL);
        struct tm *aTime = localtime(&theTime);
        int hour=aTime->tm_hour;
        int minute=aTime->tm_min;

        //std::cout << hour << ":"<< minute<< std::endl;
        int gies_t_min=0;
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

        if (trigger_regel == 1 && giessenActive == 0){
            this->writeLog("Regel-Giessen: Giessen wird getriggert.");
            this->startGiessen(gies_t_min * 60);
            usleep(2000000); // 2s warten zur entprellung des threads
        }


    }
}



