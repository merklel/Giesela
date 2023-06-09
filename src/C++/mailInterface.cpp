#include "gisela.hpp"

#define GMAIL_ADDRESS "test@gmail.com"
#define GMAIL_PW "superscret"


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}

int Gisela::checkForNewMails() {

    CURL *curl;
    CURLcode res = CURLE_OK;
    curl = curl_easy_init();
    std::string readBuffer;

    if (curl) {
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, GMAIL_ADDRESS);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, GMAIL_PW);

        /* This will fetch message 1 from the user's inbox */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");
        curl_easy_setopt(curl, CURLOPT_URL, "imaps://imap.gmail.com/INBOX;UID=*");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        //-------------CURL TUTORIAL:--------------------------------------------
        //https://securityblog.gr/4285/read-email-using-imap-gmail-c-and-libcurl/

        /* Perform the fetch */
        res = curl_easy_perform(curl);
        //std::cout << "---------- buffer:" << std::endl;
        //std::cout << readBuffer << std::endl;

        /* Check for errors in Curl */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* Always cleanup */
        curl_easy_cleanup(curl);


        // --------------- Get unseem mails using REGEX ------------------------
        std::smatch m;
        std::regex e("[[:digit:]]+");

        bool mailVorhanden = true;
        int i = 0;

        // If regex finds sth. it loops here
        while (mailVorhanden) {
            mailVorhanden = std::regex_search(readBuffer, m, e);
            if (mailVorhanden == true) {

                unseenMailId.push_back(std::stoi(m.str()));
                std::cout << "Neue Mail mit ID: " << unseenMailId.at(i) << std::endl;
                ++i;
                readBuffer = m.suffix().str();
            }
        }

        // Goes here if no mail arrived
        if(unseenMailId.empty()){
            std::cout << "Keine neue Mail vorhanden!" << std::endl;
            return 0;
        }else{
            return 1; //new mails
        }
        //-----------------------------------------------------------------------
    }
}



int Gisela::readMails() {

    if (!unseenMailId.empty()) {

        CURL *curl;
        CURLcode res = CURLE_OK;
        curl = curl_easy_init();
        std::string readBuffer;

        std::string sender;
        std::string subject;

        if (curl) {

            for(int i=0; i < unseenMailId.size(); ++i) {
                /* Set username and password */
                curl_easy_setopt(curl, CURLOPT_USERNAME, GMAIL_ADDRESS);
                curl_easy_setopt(curl, CURLOPT_PASSWORD, GMAIL_PW);

                /* This will fetch message 1 from the user's inbox */
                //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");
                std::string address = "imaps://imap.gmail.com/INBOX;UID=";
                //std::cout << address.append(std::to_string(unseenMailId.at(i))) << std::endl;
                curl_easy_setopt(curl, CURLOPT_URL, address.append(std::to_string(unseenMailId.at(i))).c_str());

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

                //-------------CURL TUTORIAL:--------------------------------------------
                //https://securityblog.gr/4285/read-email-using-imap-gmail-c-and-libcurl/

                /* Perform the fetch */
                res = curl_easy_perform(curl);
                std::cout << "---------- buffer:" << std::endl;
                //std::cout << readBuffer << std::endl;

                size_t pos_subject      = readBuffer.find("Subject:");
                size_t pos_subject_end  = readBuffer.substr(pos_subject).find("\n");

                //std::cout << "alive 1 !" << std::endl;

                size_t pos_sender       = readBuffer.find("From:");
                size_t pos_sender_end   = readBuffer.substr(pos_sender).find("\n");

                //std::cout << "alive 2 !" << std::endl;

                sender  = readBuffer.substr(pos_sender+6, pos_sender_end);
                subject = readBuffer.substr(pos_subject+9, pos_subject_end);

                //std::cout << "From: " << readBuffer.substr(pos_sender+6, pos_sender_end) << std::endl;
                //std::cout << "Subject: " << readBuffer.substr(pos_subject+9, pos_subject_end) << std::endl;

                std::cout << "From: " << sender << std::endl;
                std::cout << "Subject: " << subject << std::endl;


                // Check Mails and set giess-trigger
                got_trigger_mail = 0;
                for (int i=0; i< trustyMailAdresses.size(); ++i){
                    std::size_t f = sender.find(trustyMailAdresses.at(i));
                    if (f != std::string::npos){
                        got_trigger_mail = 1;
                    }
                }            

                readBuffer = "";

            }
            unseenMailId.resize(0);

            /* Check for errors in Curl */
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            /* Always cleanup */
            curl_easy_cleanup(curl);
        }

        return 1;
    }else{
        //std::cout << "Cant read mails if no new arrived!" << std::endl;
        return 0;
    }
}



void Gisela::MailServer_thread(){


    while(1){

        std::this_thread::sleep_for(std::chrono::seconds(10));

        checkForNewMails();

        readMails();

    }
}



void Gisela::startMailServer(){
    std::thread socketThread(&Gisela::MailServer_thread, this);
    socketThread.detach();
}


int Gisela::getTriggerMailStatus(){
    return got_trigger_mail;
}

void Gisela::resetTriggerMailStatus(){
    got_trigger_mail = 0;
}
